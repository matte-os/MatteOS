#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::String;

  class FileSystem : public RefCounted<FileSystem> {
  public:
    FileSystem() = default;
    virtual ~FileSystem() = default;

    virtual String name() const = 0;
    virtual ErrorOr<RefPtr<Inode>> root() = 0;

    virtual ErrorOr<RefPtr<Inode>> open() = 0;
    virtual ErrorOr<void> close(RefPtr<Inode> inode) = 0;
  };
}// namespace Kernel