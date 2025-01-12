#pragma once

#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/RAMFS/RamInode.h>

namespace Kernel {
  class RamFileSystem final : public FileSystem {
    RefPtr<RamInode> m_root;

  public:
    RamFileSystem() = default;
    ~RamFileSystem() override = default;

    String name() const override { return "ramfs"; };
    ErrorOr<RefPtr<Inode>> root() override;

    ErrorOr<RefPtr<OpenFileDescriptor>> open(const Credentials& credentials, StringView path, FileOpenMode mode) override;
    ErrorOr<void> close(RefPtr<OpenFileDescriptor> inode) override;
    bool exists(StringView path) override;
  };
}// namespace Kernel
