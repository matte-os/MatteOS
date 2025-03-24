#include <Kernel/FileSystem/RAMFS/RamFileSystem.h>
#include <Kernel/Devices/Device.h>

namespace Kernel {
  using Utils::Error;

  RamFileSystem::RamFileSystem() {
    m_root = RamInode::create_directory();
  }

  ErrorOr<RefPtr<Inode>> RamFileSystem::root() {
    if(!m_root) {
      return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Root inode not found!"));
    }

    return ErrorOr<RefPtr<Inode>>::create(m_root);
  }

  ErrorOr<RefPtr<OpenFileDescriptor>> RamFileSystem::open(const Credentials& credentials, StringView path, FileOpenMode mode) {
    return FileSystem::open(credentials, path, mode);
  }

  ErrorOr<void> RamFileSystem::close(RefPtr<OpenFileDescriptor> inode) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented!"));
  }

  bool RamFileSystem::exists(StringView path) {
    return m_root->get_child(path).has_value();
  }
}// namespace Kernel