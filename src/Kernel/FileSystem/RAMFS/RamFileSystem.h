#pragma once

#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/RAMFS/RamInode.h>

namespace Kernel {
  class RamFileSystem : public FileSystem {
  private:
    RefPtr<RamInode> m_root;

  public:
    RamFileSystem() = default;
    virtual ~RamFileSystem() = default;

    virtual String name() const override { return "ramfs"; };
    virtual ErrorOr<RefPtr<Inode>> root() override;

    ErrorOr<RefPtr<OpenFileDescriptor>> open(const Credentials& credentials, StringView path, FileOpenMode mode) override;
    virtual ErrorOr<void> close(RefPtr<OpenFileDescriptor> inode) override;
  };
}// namespace Kernel
