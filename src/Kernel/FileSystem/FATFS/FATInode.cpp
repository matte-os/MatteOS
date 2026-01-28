/**
 * @file FATInode.cpp
 * @author Matěj Bucek
 */
#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Kernel/FileSystem/FATFS/FATInode.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/Request.h>
#include <Utils/Math.h>

namespace Kernel {
  FATInode::FATInode(RefPtr<FATFileSystem>&& fs, FAT::FullDirectoryEntry entry, String&& name) : m_fs(move(fs)), m_entry(entry), m_name(move(name)) {}

  FATInode::FATInode(RefPtr<FATFileSystem>& fs, FAT::FullDirectoryEntry entry, String&& name) : m_fs(fs), m_entry(entry), m_name(move(name)) {}

  RefPtr<FileSystem> FATInode::fs() {
    return m_fs;
  }

  ErrorOr<RefPtr<Inode>> FATInode::get_child(StringView name) {
    if(!m_entry.is_directory()) {
      return Error::create_from_string("Not a directory");
    }

    auto block = TRY(m_fs->read_cluster_poll(m_entry.first_cluster));
    // FIXME: Add support for FAT linked clusters
    for(size_t directory_entry_index = 0; directory_entry_index < m_fs->get_block_size() / sizeof(FAT::DirectoryEntry); directory_entry_index++) {
      auto directory_entry = reinterpret_cast<FAT::DirectoryEntry*>(block->data() + directory_entry_index * sizeof(FAT::DirectoryEntry));
      if(directory_entry->is_free()) {
        continue;
      }

      String entry_name;
      if(directory_entry->is_long_name()) {
        auto* long_entry = directory_entry->as_long_name();
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

          entry_name = tmp + entry_name;

          if(long_entry->is_final_entry() || long_entry->is_free()) {
            directory_entry_index++;
            directory_entry = reinterpret_cast<FAT::DirectoryEntry*>(block->data() + directory_entry_index * sizeof(FAT::DirectoryEntry));
            if(directory_entry->is_long_name()) {
              return Error::create_from_string("Corrupt directory: Last Long name entry not followed by standard entry");
            }

            break;
          }

          long_entry++;
          directory_entry_index++;
        }
      } else {
        entry_name = String(reinterpret_cast<char*>(directory_entry->name), 11);
      }

      if(directory_entry->is_long_name()) {
        return Error::create_from_string("Corrupt directory: Long name entry without standard entry");
      }

      if(entry_name == name) {
        return RefPtr<Inode>(new FATInode(m_fs, FAT::FullDirectoryEntry::filled_from(entry_name, *directory_entry), move(entry_name)));
      }
    }

    return Error::create_from_string("Child not found");
  }

  ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> FATInode::list_dir() {
    ArrayList<RefPtr<DirectoryEntry>> entries;

    if(!m_entry.is_directory()) {
      return Error::create_from_string("Not a directory");
    }

    auto block = TRY(m_fs->read_cluster_poll(m_entry.first_cluster));
    // FIXME: Add support for FAT linked clusters
    for(size_t directory_entry_index = 0; directory_entry_index < m_fs->get_block_size() / sizeof(FAT::DirectoryEntry); directory_entry_index++) {
      auto directory_entry = reinterpret_cast<FAT::DirectoryEntry*>(block->data() + directory_entry_index * sizeof(FAT::DirectoryEntry));
      if(directory_entry->is_free()) {
        continue;
      }

      String entry_name;
      if(directory_entry->is_long_name()) {
        auto* long_entry = directory_entry->as_long_name();
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


          entry_name = tmp + entry_name;

          if(long_entry->is_final_entry() || long_entry->is_free()) {
            directory_entry_index++;
            break;
          }

          long_entry++;
          directory_entry_index++;
        }
      } else {
        entry_name = String(reinterpret_cast<char*>(directory_entry->name), 11);
      }

      entries.add(RefPtr(new DirectoryEntry(entry_name)));
    }

    return entries;
  }

  ErrorOr<void> FATInode::check_credentials(const Credentials& credentials, const FileOpenMode& mode) {
    return ErrorOr<void>::create({});
  }

  ErrorOr<Request<size_t>> FATInode::read(u8* buffer, size_t size, size_t offset) {
    // EOF
    if(offset >= m_entry.size) {
      return Request<size_t>::create_finalized(0);
    }

    // Cap the size
    if(offset + size > m_entry.size) {
      size = m_entry.size - offset;
    }

    size_t bytes_read = 0;
    size_t current_file_offset = offset;

    u32 current_cluster = m_entry.first_cluster;
    auto cluster_size_in_bytes = m_fs->m_bytes_per_cluster;

    auto clusters_to_skip = offset / cluster_size_in_bytes;
    auto initial_cluster_offset = offset % cluster_size_in_bytes;

    for(size_t i = 0; i < clusters_to_skip; i++) {
      auto cluster_entry = TRY(m_fs->get_fat_entry(current_cluster));
      if(cluster_entry.is_end_of_chain()) {
        return Error::create_from_string("Corrupt FAT chain: Reached EOF before offset");
      }
      current_cluster = cluster_entry.value;
    }

    while(bytes_read < size) {
      auto cluster_data_block = TRY(m_fs->read_cluster_poll(current_cluster));
      size_t offset_in_cluster = (bytes_read == 0) ? initial_cluster_offset : 0;
      size_t available_bytes_in_cluster = cluster_size_in_bytes - offset_in_cluster;
      size_t bytes_to_read = Utils::min(size - bytes_read, available_bytes_in_cluster);

      Utils::memcpy(reinterpret_cast<char*>(buffer + bytes_read), reinterpret_cast<const char*>(cluster_data_block->data() + offset_in_cluster), bytes_to_read);

      bytes_read += bytes_to_read;

      if(bytes_read < size) {
        auto cluster_entry = TRY(m_fs->get_fat_entry(current_cluster));
        if(cluster_entry.is_end_of_chain()) {
          break;
        }
        current_cluster = cluster_entry.value;
      }
    }

    return Request<size_t>::create_finalized(bytes_read);
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