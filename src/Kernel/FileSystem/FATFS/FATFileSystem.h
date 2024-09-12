//
// Created by matejbucek on 28.8.24.
//

#pragma once

#include <Kernel/FileSystem/BlockBackedFileSystem.h>
#include <Kernel/FileSystem/FATFS/FAT.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/OpenFileDescriptor.h>

namespace Kernel {

  enum class FATType {
    FAT12,
    FAT16,
    FAT32
  };

  class FATFileSystem : public BlockBackedFileSystem {
  private:
    OpenFileDescriptorTable m_open_files;
    FATType m_fat_type;
    BootSector* m_fat_boot_sector;

    FATFileSystem(RefPtr<Device> device, FATType fat_type, BootSector* fat_boot_sector) : BlockBackedFileSystem(device), m_fat_type(fat_type), m_fat_boot_sector(fat_boot_sector) {}

  public:
    static ErrorOr<RefPtr<FileSystem>> try_create(RefPtr<Device> device);

    String name() const override;
    ErrorOr<RefPtr<Inode>> root() override;
    ErrorOr<RefPtr<Inode>> open() override;
    ErrorOr<void> close(RefPtr<Inode> inode) override;
    ~FATFileSystem() override;
  };

}// namespace Kernel
