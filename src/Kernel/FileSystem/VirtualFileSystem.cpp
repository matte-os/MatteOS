#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Utils/Assertions.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::Error;
using Utils::ErrorOr;

namespace Kernel {
  static VirtualFileSystem* s_instance = nullptr;

  void VirtualFileSystem::init() {
    if(!s_instance) {
      s_instance = new VirtualFileSystem();
    }
  }

  VirtualFileSystem& VirtualFileSystem::the() {
    runtime_assert(s_instance, "VirtualFileSystem not initialized");
    return *s_instance;
  }

  ErrorOr<RefPtr<Inode>> VirtualFileSystem::open(const Credentials& credentials, const String& path, FileOpenMode mode) {
    auto path_parts = path.split("/");
    auto inode = m_root_inode;

    if(!inode) {
      return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("No root fs mounted!"));
    }

    for(size_t i = 0; i < path_parts.size(); i++) {
      auto error_or_next = inode->lookup(path_parts[i]);
      if(error_or_next.has_error()) {
        if(i == path_parts.size() - 1 && (mode.is<FileOperationMode::Create>() && mode.is<FileOperationMode::Exclusive>())) {
          // TODO: Create a new file
          return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Not implemented"));
        } else {
          return error_or_next;
        }
      }

      inode = error_or_next.get_value();
    }

    auto can_access = inode->check_credentials(credentials, mode);
    if(can_access.has_error()) {
      return ErrorOr<RefPtr<Inode>>::create_error(can_access.get_error());
    }

    if(mode.is<FileOperationMode::Exclusive>()) {
      return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Cannot open file in exclusive mode! File already exists!"));
    }

    return ErrorOr<RefPtr<Inode>>::create(inode);
  }
  ErrorOr<void> VirtualFileSystem::close(RefPtr<Inode> inode) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented"));

  }
}// namespace Kernel