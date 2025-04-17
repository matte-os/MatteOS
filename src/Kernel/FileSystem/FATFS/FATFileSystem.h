/**
 * @file FATFileSystem.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/FileSystem/FATFS/FATInode.h>
#include <Kernel/FileSystem/BlockBackedFileSystem.h>
#include <Kernel/FileSystem/FATFS/FAT.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/OpenFileDescriptor.h>

namespace Kernel {

  using FAT::BootSector;
  using FAT::BootSector32;
  using FAT::DirectoryEntryAttributes;

  enum class FATType {
    FAT12,
    FAT16,
    FAT32
  };

  class FATFileSystem : public BlockBackedFileSystem {
    friend class FATInode;
    FATType m_fat_type;
    BootSector* m_fat_boot_sector;
    RefPtr<FATInode> m_root;

    // Fat specific fields
    size_t m_bytes_per_cluster;
    size_t m_reserved_and_fat_offset;
    size_t m_fat_start_offset;

    size_t m_number_of_fat_blocks;

    FATFileSystem(RefPtr<Device> device, FATType fat_type, BootSector* fat_boot_sector) : BlockBackedFileSystem(device, fat_boot_sector->sectors_per_cluster * fat_boot_sector->bytes_per_sector), m_fat_type(fat_type), m_fat_boot_sector(fat_boot_sector) {
      if(m_fat_type == FATType::FAT32) {
        auto fat32 = reinterpret_cast<BootSector32*>(fat_boot_sector);
        m_root = {new FATInode({this}, {
                                               .attributes = as_underlying(DirectoryEntryAttributes::Directory),
                                               .name = {0},
                                               .first_cluster_high = 0,
                                               .first_cluster_low = fat32->root_cluster,
                                       },
                               "")};

        m_bytes_per_cluster = fat32->sectors_per_cluster * fat32->bytes_per_sector;
        m_fat_start_offset = fat32->reserved_sectors * fat32->bytes_per_sector;
        m_reserved_and_fat_offset = m_fat_start_offset + fat32->fat_count * fat32->sectors_per_fat32 * fat32->bytes_per_sector;
        m_number_of_fat_blocks = (m_reserved_and_fat_offset - m_fat_start_offset) / m_bytes_per_cluster;
      }

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

  protected:
    ErrorOr<RefPtr<DataBlock>> read_block_poll(size_t block_number) override;
    ErrorOr<void> write_block_poll(size_t block_number, char* buffer) override;
    ErrorOr<void> read_block_request() override;
    ErrorOr<void> write_block_request() override;

    ErrorOr<RefPtr<DataBlock>> read_cluster_poll(size_t cluster);
  public:
    static ErrorOr<RefPtr<FileSystem>> try_create(RefPtr<Device> device);

    String name() const override;
    ErrorOr<RefPtr<Inode>> root() override;
    ErrorOr<RefPtr<OpenFileDescriptor>> open(const Credentials& credentials, StringView path, FileOpenMode mode) override;
    ErrorOr<void> close(RefPtr<OpenFileDescriptor> inode) override;
    bool exists(StringView path) override;
    ErrorOr<void> flush();
    ~FATFileSystem() override;

  private:
    ErrorOr<FAT::DirectoryEntry*> get_root_directory();
    ErrorOr<String> get_name(FAT::DirectoryEntry* entry);
  };

}// namespace Kernel
