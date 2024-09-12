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
  }

  ErrorOr<RefPtr<Inode>> FATFileSystem::open() {

  }

  ErrorOr<void> FATFileSystem::close(RefPtr<Inode> inode) {
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
      auto boot_sector = reinterpret_cast<BootSector32*>(buffer);
      delete buffer;
      return ErrorOr<RefPtr<FileSystem>>::create(RefPtr<FATFileSystem>(new FATFileSystem(device, FATType::FAT32, boot_sector)));
    }
  }

  FATFileSystem::~FATFileSystem() {
    delete m_fat_boot_sector;
  }
}// namespace Kernel