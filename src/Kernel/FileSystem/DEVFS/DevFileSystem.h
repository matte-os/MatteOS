#pragma once

#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/Inode.h>

namespace Kernel {

  class DevFileSystem : public FileSystem {
  private:
    RefPtr<DevInode> m_root;

  public:
    DevFileSystem() = default;
    virtual ~DevFileSystem() = default;

    virtual String name() const override { return "devfs"; }
  };

}// namespace Kernel
