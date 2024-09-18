//
// Created by matejbucek on 28.8.24.
//

#pragma once

#include <Kernel/FileSystem/FATFS/FAT.h>
#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Forwards.h>

namespace Kernel {

  class FATInode final : public Inode {
  private:
    RefPtr<FATFileSystem> m_fs;
    DirectoryEntry m_entry;
    String m_name;

  public:
    FATInode(RefPtr<FATFileSystem>&& fs, DirectoryEntry entry, String&& name);

    ErrorOr<StringView> name() override {
      return ErrorOr<StringView>::create(m_name);
    }

    ~FATInode() override = default;
    ErrorOr<void> check_credentials(const Credentials& credentials, const FileOpenMode& mode) override;
    ErrorOr<size_t> read(u8* buffer, size_t size, size_t offset) override;
    ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) override;
    RefPtr<FileSystem> fs() override;
    ErrorOr<RefPtr<Inode>> lookup(const String& name) override;
    ErrorOr<Array<RefPtr<Inode>>> list_dir() override;
    bool is_directory() override;
    bool is_file() override;
  };

}// namespace Kernel
