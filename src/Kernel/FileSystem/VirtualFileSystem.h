#pragma once

namespace Kernel {
  class VirtualFileSystem {
  public:
    VirtualFileSystem() = default;
    static void init();
    static VirtualFileSystem& the();
    ~VirtualFileSystem() = default;
  };
}// namespace Kernel