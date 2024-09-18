//
// Created by matejbucek on 28.8.24.
//

#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Kernel/FileSystem/FATFS/FATInode.h>

namespace Kernel {
  FATInode::FATInode(RefPtr<FATFileSystem>&& fs, DirectoryEntry entry, String&& name) : m_fs(move(fs)), m_entry(entry), m_name(move(name)) {}

  RefPtr<FileSystem> FATInode::fs() {
    return m_fs;
  }

  ErrorOr<RefPtr<Inode>> FATInode::lookup(const String& name) {

  }

  ErrorOr<Array<RefPtr<Inode>>> FATInode::list_dir() {

  }

  ErrorOr<void> FATInode::check_credentials(const Credentials& credentials, const FileOpenMode& mode) {

  }

  ErrorOr<size_t> FATInode::read(u8* buffer, size_t size, size_t offset) {

  }

  ErrorOr<size_t> FATInode::write(const u8* buffer, size_t size, size_t offset) {

  }

  bool FATInode::is_directory() {
    return m_entry.attributes & as_underlying(DirectoryEntryAttributes::Directory);
  }

  bool FATInode::is_file() {
    return !is_directory();
  }

}// namespace Kernel