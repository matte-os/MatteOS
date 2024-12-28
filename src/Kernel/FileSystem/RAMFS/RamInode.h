#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::HashMap;
  using Utils::String;

  class RamInode : public Inode {
  private:
    HashMap<String, RefPtr<Inode>> m_children;
    RefPtr<RamFileSystem> m_fs;

  public:
    RamInode() = default;
    virtual ~RamInode() = default;

    virtual ErrorOr<size_t> read(u8* buffer, size_t size, size_t offset) override;
    virtual ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) override;
    virtual RefPtr<FileSystem> fs() override;
  };
}// namespace Kernel
