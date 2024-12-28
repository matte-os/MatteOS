//
// Created by matejbucek on 28.8.24.
//

#include <Kernel/Devices/Device.h>
#include <Kernel/FileSystem/FATFS/FAT.h>
#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Utils/Memory.h>

namespace Kernel {
  using Utils::memcpy;

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
    String path_so_far = "";
    while(node_index < path_parts.size()) {
      // Check if the inode is a directory
      if(!inode->is_directory()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Not a directory"));
      }

      auto& name = path_parts[node_index];
      auto directory_entries = TRY(inode->list_dir());

      auto contains_path_part = directory_entries.contains([name](auto entry) {
        return entry->get_name() == name;
      });

      if(!contains_path_part) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Path not found"));
      }

      path_so_far += "/";
      path_so_far += name;

      RefPtr<Inode> new_inode;
      DebugConsole::print("Path so far: ");
      DebugConsole::println(path_so_far);

      if(m_open_files.has_descriptor(path_so_far)) {
        new_inode = TRY(m_open_files.open(path_so_far))->inode();
      } else {
        new_inode = TRY(inode->get_child(name));
        m_open_files.create_descriptor_and_open(path_so_far, new_inode);
        DebugConsole::print("Created descriptor for: ");
        DebugConsole::println(path_so_far);
      }

      if(m_root != inode) {
        m_open_files.close(path_so_far);
      }

      inode = new_inode;
      node_index++;
    }

    DebugConsole::print("Opened file: ");
    DebugConsole::println(path);
    return TRY(m_open_files.get_descriptor(path));
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

    auto error_or_cluster = read_cluster_poll(fat32->root_cluster);

    if(error_or_cluster.has_error()) {
      return ErrorOr<FAT::DirectoryEntry*>::create_error(error_or_cluster.get_error());
    }

    auto buffer = new u8[m_bytes_per_cluster];
    auto cluster = error_or_cluster.get_value();
    memcpy(reinterpret_cast<char*>(buffer), reinterpret_cast<const char*>(cluster->data()), m_bytes_per_cluster);

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

      if(long_entry->is_final_entry() || long_entry->is_free()) {
        break;
      }

      long_entry++;
    }

    return ErrorOr<String>::create(name);
  }

  bool FATFileSystem::exists(StringView path) {
    return false;
  }

  ErrorOr<RefPtr<DataBlock>> FATFileSystem::read_block_poll(size_t block_number) {
    auto maybe_block = get_block_from_cache(block_number);

    // If we found the block in the cache, return it.
    if(maybe_block) {
      cache(maybe_block.get_value());
      return maybe_block;
    }

    // Otherwise, read the block from the device.
    auto block = RefPtr<DataBlock>::create(block_number, get_block_size());
    auto buffer = block->data();

    // We have to convert the block number by adding the fat start offset
    auto read_or_error = get_device()->read_poll(buffer, get_block_size(), m_fat_start_offset + block_number * get_block_size());
    TRY(read_or_error);

    cache(block);

    return block;
  }

  ErrorOr<void> FATFileSystem::write_block_poll(size_t block_number, char* buffer) {
  }

  ErrorOr<void> FATFileSystem::read_block_request() {
  }

  ErrorOr<void> FATFileSystem::write_block_request() {
  }

  ErrorOr<RefPtr<DataBlock>> FATFileSystem::read_cluster_poll(size_t cluster) {
    auto block_number = m_number_of_fat_blocks + cluster - 2;
    return read_block_poll(block_number);
  }
}// namespace Kernel