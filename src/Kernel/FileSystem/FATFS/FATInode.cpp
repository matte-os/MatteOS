/**
 * @file FATInode.cpp
 * @author Matěj Bucek
 */
#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Kernel/FileSystem/FATFS/FATInode.h>
#include <Kernel/Process/Request.h>

namespace Kernel {
  FATInode::FATInode(RefPtr<FATFileSystem>&& fs, FAT::DirectoryEntry entry, String&& name) : m_fs(move(fs)), m_entry(entry), m_name(move(name)) {}
  FATInode::FATInode(RefPtr<FATFileSystem>& fs, FAT::DirectoryEntry entry, String&& name) : m_fs(fs), m_entry(entry), m_name(move(name)) {}

  RefPtr<FileSystem> FATInode::fs() {
    return m_fs;
  }

  ErrorOr<RefPtr<Inode>> FATInode::get_child(Utils::StringView name) {
    auto block = TRY(m_fs->read_cluster_poll(m_entry.first_cluster_low));
    // FIXME: Add support for FAT linked clusters
    for(size_t directory_entry_index = 0; directory_entry_index < m_fs->get_block_size() / sizeof(FAT::DirectoryEntry); directory_entry_index++) {
      auto directory_entry = reinterpret_cast<FAT::DirectoryEntry*>(block->data() + directory_entry_index * sizeof(FAT::DirectoryEntry));
      if(directory_entry->is_free()) {
        continue;
      }

      auto entry_name = TRY(m_fs->get_name(directory_entry));

      if(entry_name == name) {
        return RefPtr<Inode>(new FATInode(m_fs, *directory_entry, move(entry_name)));
      }
    }

    return Error::create_from_string("Child not found");
  }

  ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> FATInode::list_dir() {
    ArrayList<RefPtr<DirectoryEntry>> entries;

    auto block = TRY(m_fs->read_cluster_poll(m_entry.first_cluster_low));
    // FIXME: Add support for FAT linked clusters
    for(size_t directory_entry_index = 0; directory_entry_index < m_fs->get_block_size() / sizeof(FAT::DirectoryEntry); directory_entry_index++) {
      auto directory_entry = reinterpret_cast<FAT::DirectoryEntry*>(block->data() + directory_entry_index * sizeof(FAT::DirectoryEntry));
      if(directory_entry->is_free()) {
        continue;
      }

      auto name = TRY(m_fs->get_name(directory_entry));

      entries.add(RefPtr<DirectoryEntry>(new DirectoryEntry(name)));
    }

    return entries;
  }

  ErrorOr<void> FATInode::check_credentials(const Credentials& credentials, const FileOpenMode& mode) {
    return ErrorOr<void>::create({});
  }

  ErrorOr<Request<size_t>> FATInode::read(u8* buffer, size_t size, size_t offset) {
    return Error::create_from_string("Not implemented");
  }

  ErrorOr<size_t> FATInode::write(const u8* buffer, size_t size, size_t offset) {
    return Error::create_from_string("Not implemented");
  }

  bool FATInode::is_directory() {
    return m_entry.attributes & as_underlying(DirectoryEntryAttributes::Directory);
  }

  bool FATInode::is_file() {
    return !is_directory();
  }
}// namespace Kernel