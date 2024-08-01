#include <Kernel/FileSystem/RAMFS/RamFileSystem.h>

namespace Kernel {
  using Utils::Error;

  ErrorOr<RefPtr<Inode>> RamFileSystem::root() {
    if(!m_root) {
      return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Root inode not found!"));
    }

    return ErrorOr<RefPtr<Inode>>::create(static_cast<RefPtr<Inode>>(m_root));
  }

  ErrorOr<RefPtr<Inode>> RamFileSystem::open() {
    return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Not implemented!"));
  }

  ErrorOr<void> RamFileSystem::close(RefPtr<Inode> inode) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented!"));
  }
}// namespace Kernel