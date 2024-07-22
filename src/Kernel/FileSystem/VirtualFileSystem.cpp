#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Utils/Assertions.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::ErrorOr;
using Utils::Error;

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

  ErrorOr<RefPtr<Inode>> VirtualFileSystem::open(const Credentials& credentials, const StringView& path, FileOpenMode mode) {
    return ErrorOr<RefPtr<Inode>>::create_error(Error::create_from_string("Not implemented"));
  }
}// namespace Kernel