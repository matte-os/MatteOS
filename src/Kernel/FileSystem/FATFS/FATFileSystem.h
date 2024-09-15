//
// Created by matejbucek on 28.8.24.
//

#pragma once

#include "FATInode.h"
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
    RefPtr<FATInode> m_root;


    FATFileSystem(RefPtr<Device> device, FATType fat_type, BootSector* fat_boot_sector) : BlockBackedFileSystem(device), m_fat_type(fat_type), m_fat_boot_sector(fat_boot_sector) {
      m_root = {new FATInode({this}, DirectoryEntry(), "")};

      auto error_or_root = get_root_directory();


      if(error_or_root.has_error()) {
        DebugConsole::print("Failed to get root directory: ");
        DebugConsole::println(error_or_root.get_error().get_message());
      } else {
        auto root = error_or_root.get_value();
        DebugConsole::print("Root directory first file: ");
        auto& root_entry = root[0];
        if(!root_entry.is_long_name()) {
          DebugConsole::println(reinterpret_cast<char*>(root[0].name));
        } else {
          auto error_or_name = get_name(&root_entry);
          DebugConsole::println(error_or_name.get_value());
        }
        delete root;
      }
    }

  public:
    static ErrorOr<RefPtr<FileSystem>> try_create(RefPtr<Device> device);

    String name() const override;
    ErrorOr<RefPtr<Inode>> root() override;
    ErrorOr<RefPtr<Inode>> open() override;
    ErrorOr<void> close(RefPtr<Inode> inode) override;
    ~FATFileSystem() override;

  private:
    ErrorOr<DirectoryEntry*> get_root_directory();
    ErrorOr<String> get_name(DirectoryEntry* entry);
  };

}// namespace Kernel
