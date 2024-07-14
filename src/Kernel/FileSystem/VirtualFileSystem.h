#pragma once

#include <Utils/Errors/ErrorOr.h>
#include <Kernel/FileSystem/Inode.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::Pointers::RefPtr;
  using Utils::ErrorOr;
  using Utils::Strings::StringView;

  class VirtualFileSystem {
  private:
    RefPtr<Inode> m_root_inode;

  public:
    VirtualFileSystem() = default;
    static void init();
    static VirtualFileSystem& the();
    ~VirtualFileSystem() = default;

    ErrorOr<void> mount(const StringView& path, RefPtr<Inode> inode);
    ErrorOr<void> unmount(const StringView& path);
    ErrorOr<RefPtr<Inode>> open(const StringView& path);
    ErrorOr<void> close(RefPtr<Inode> inode);
  };
}// namespace Kernel