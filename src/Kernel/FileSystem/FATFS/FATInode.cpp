//
// Created by matejbucek on 28.8.24.
//

#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Kernel/FileSystem/FATFS/FATInode.h>

namespace Kernel {
  FATInode::FATInode(RefPtr<FATFileSystem>&& fs, FAT::DirectoryEntry entry, String&& name) : m_fs(move(fs)), m_entry(entry), m_name(move(name)) {}

  RefPtr<FileSystem> FATInode::fs() {
    return m_fs;
  }

  ErrorOr<RefPtr<Inode>> FATInode::lookup(const String& name) {
    return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Lookup implemented"));
  }

  ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> FATInode::list_dir() {
    return ErrorOr<ArrayList<RefPtr<DirectoryEntry>>>::create_error(Error::create_from_string("List dir not implemented"));
  }

  ErrorOr<void> FATInode::check_credentials(const Credentials& credentials, const FileOpenMode& mode) {
    return ErrorOr<void>::create({});
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