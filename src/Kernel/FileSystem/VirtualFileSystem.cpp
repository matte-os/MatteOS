#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Utils/Assertions.h>

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

  }
}// namespace Kernel