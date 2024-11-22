//
// Created by matejbucek on 28.8.24.
//

#include <Kernel/Devices/Device.h>
#include <Kernel/FileSystem/FATFS/FAT.h>
#include <Kernel/FileSystem/FATFS/FATFileSystem.h>

namespace Kernel {
  String FATFileSystem::name() const {
    return "FAT";
  }

  ErrorOr<RefPtr<Inode>> FATFileSystem::root() {
    if(m_root) {
      return ErrorOr<RefPtr<Inode>>::create(m_root);
    }

    return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Root inode not found"));
  }

  ErrorOr<RefPtr<OpenFileDescriptor>> FATFileSystem::open(const Credentials& credentials, StringView path, FileOpenMode mode) {
    // Check if the file is already open in the open file table
    if(m_open_files.has_descriptor(path)) {
      return m_open_files.open(path);
    }

    // If the file is not open, open it and add it to the open file table
    //FIXME: StringView should have split too
    auto path_parts = String(path).split("/");

    // If the path is empty, put the root inode into the open file table
    // and return the open file descriptor
    if(path_parts.size() == 0) {
      auto error_or_open_file_descriptor = m_open_files.create_descriptor_and_open(path, m_root);
      if(error_or_open_file_descriptor.has_error()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_open_file_descriptor.get_error());
      }

      return ErrorOr<RefPtr<OpenFileDescriptor>>::create(error_or_open_file_descriptor.get_value());
    }

    // Use the root inode to list the first directory entries
    auto inode = m_root;
    size_t node_index = 0;
    while(node_index < path_parts.size()) {
      // Check if the inode is a directory
      if(!inode->is_directory()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Not a directory"));
      }

      auto& name = path_parts[node_index];
      auto error_or_directory_entries = inode->list_dir();
      if(error_or_directory_entries.has_error()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_directory_entries.get_error());
      }

      auto directory_entries = error_or_directory_entries.get_value();
      auto contains_path_part = directory_entries.contains([name](auto entry) {
        return entry->get_name() == name;
      });

      if(!contains_path_part) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Path not found"));
      }

      auto error_or_inode = inode->lookup(name);
      if(error_or_inode.has_error()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_inode.get_error());
      }

      inode = error_or_inode.get_value();
    }

    auto error_or_open_file_descriptor = m_open_files.create_descriptor_and_open(path, inode);
    if(error_or_open_file_descriptor.has_error()) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_open_file_descriptor.get_error());
    }

    return ErrorOr<RefPtr<OpenFileDescriptor>>::create(error_or_open_file_descriptor.get_value());
  }

  ErrorOr<void> FATFileSystem::close(RefPtr<OpenFileDescriptor> inode) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented"));
  }

  ErrorOr<RefPtr<FileSystem>> FATFileSystem::try_create(RefPtr<Device> device) {
    auto block_device = device->as<BlockDevice>();

    auto buffer = new u8[512];
    auto read_result = block_device->read_poll(buffer, 512, 0);
    if(read_result.has_error()) {
      return ErrorOr<RefPtr<FileSystem>>::create_error(read_result.get_error());
    }

    auto fat = reinterpret_cast<BootSector*>(buffer);
    auto fat32 = reinterpret_cast<BootSector32*>(buffer);

    // First, determine the count of sectors occupied by the root directory:
    auto root_dir_sectors = (fat->root_entries * 32 + (fat->bytes_per_sector - 1)) / fat->bytes_per_sector;

    // Next, we determine the count of sectors in the data region of the volume:
    auto sectors_per_fat = (fat->sectors_per_fat != 0) ? fat->sectors_per_fat : fat32->sectors_per_fat32;
    auto total_sectors = (fat->total_sectors != 0) ? fat->total_sectors : fat->total_sectors_large;
    auto data_sectors = total_sectors - (fat->reserved_sectors + fat->fat_count * sectors_per_fat + root_dir_sectors);

    // Lastly, determine the count of clusters as:
    auto count_of_clusters = data_sectors / fat->sectors_per_cluster;
    if(count_of_clusters < 4085) {
      delete buffer;
      return ErrorOr<RefPtr<FileSystem>>::create_error(Error::create_from_string("FAT12 is not supported"));
    } else if(count_of_clusters < 65525) {
      delete buffer;
      return ErrorOr<RefPtr<FileSystem>>::create_error(Error::create_from_string("FAT16 is not supported"));
    } else {
      auto boot_sector = new BootSector32(*reinterpret_cast<BootSector32*>(buffer));
      delete buffer;
      return ErrorOr<RefPtr<FileSystem>>::create(RefPtr<FATFileSystem>(new FATFileSystem(device, FATType::FAT32, boot_sector)));
    }
  }

  FATFileSystem::~FATFileSystem() {
    delete m_fat_boot_sector;
  }

  ErrorOr<FAT::DirectoryEntry*> FATFileSystem::get_root_directory() {
    if(m_fat_type != FATType::FAT32) {
      return ErrorOr<FAT::DirectoryEntry*>::create_error(Error::create_from_string("Only FAT32 is supported"));
    }

    auto fat32 = reinterpret_cast<BootSector32*>(m_fat_boot_sector);

    auto bytes_per_cluster = fat32->sectors_per_cluster * fat32->bytes_per_sector;
    auto reserved_and_fat_offset = fat32->reserved_sectors * fat32->bytes_per_sector + fat32->fat_count * fat32->sectors_per_fat32 * fat32->bytes_per_sector;

    auto root_cluster_offset = reserved_and_fat_offset + (fat32->root_cluster - 2) * bytes_per_cluster;

    DebugConsole::print("Root cluster offset: ");
    DebugConsole::print_ln_number(root_cluster_offset, 16);

    auto buffer = new u8[bytes_per_cluster];
    auto read_result = get_device()->read_poll(buffer, bytes_per_cluster, root_cluster_offset);
    if(read_result.has_error()) {
      delete buffer;
      return ErrorOr<FAT::DirectoryEntry*>::create_error(read_result.get_error());
    }

    auto root_dir = reinterpret_cast<FAT::DirectoryEntry*>(buffer);
    return ErrorOr<FAT::DirectoryEntry*>::create(root_dir);
  }

  ErrorOr<String> FATFileSystem::get_name(FAT::DirectoryEntry* entry) {
    if(!entry->is_long_name()) {
      return ErrorOr<String>::create(String(reinterpret_cast<char*>(entry->name), 11));
    }

    String name;
    auto* long_entry = entry->as_long_name();
    while(true) {
      String tmp;
      for(size_t i = 0; i < 5; i++) {
        if(long_entry->name1[i] == 0 || long_entry->name1[i] == 0xFFFF) {
          break;
        }
        tmp += static_cast<char>(long_entry->name1[i] & 0xFF00 >> 8);
      }

      for(size_t i = 0; i < 6; i++) {
        if(long_entry->name2[i] == 0 || long_entry->name2[i] == 0xFFFF) {
          break;
        }
        tmp += static_cast<char>(long_entry->name2[i] & 0xFF00 >> 8);
      }

      for(size_t i = 0; i < 2; i++) {
        if(long_entry->name3[i] == 0 || long_entry->name3[i] == 0xFFFF) {
          break;
        }
        tmp += static_cast<char>(long_entry->name3[i] & 0xFF00 >> 8);
      }

      name = tmp + name;

      if((long_entry->order & 0x40)) {
        break;
      }

      long_entry++;
    }

    return ErrorOr<String>::create(name);
  }

  bool FATFileSystem::exists(StringView path) {
    return false;
  }
}// namespace Kernel