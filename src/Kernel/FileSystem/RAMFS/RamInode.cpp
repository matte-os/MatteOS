#include <Kernel/FileSystem/RAMFS/RamFileSystem.h>
#include <Kernel/FileSystem/RAMFS/RamInode.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  using Utils::Error;

  RefPtr<FileSystem> RamInode::fs() {
    return static_cast<RefPtr<FileSystem>>(m_fs);
  }
  ErrorOr<size_t> RamInode::read(u8* buffer, size_t size, size_t offset) {
    return ErrorOr<size_t>::create_error(Error::create_from_string("Not implemented!"));
  }
  ErrorOr<size_t> RamInode::write(const u8* buffer, size_t size, size_t offset) {
    return ErrorOr<size_t>::create_error(Error::create_from_string("Not implemented!"));
  }
  ErrorOr<RefPtr<Inode>> RamInode::lookup(const String& name) {
    return m_children.get(name);
  }
}// namespace Kernel