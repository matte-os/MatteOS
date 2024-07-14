#include <Kernel/FileSystem/VirtualFileSystem.h>

namespace Kernel {
  static VirtualFileSystem* g_instance = nullptr;

  void VirtualFileSystem::init() {
    if(!g_instance) {
      g_instance = new VirtualFileSystem();
    }
  }

  VirtualFileSystem& VirtualFileSystem::the() {
    return *g_instance;
  }
}