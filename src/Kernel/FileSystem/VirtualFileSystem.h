#pragma once

#include <Kernel/FileSystem/FileOpenMode.h>
#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Security/Credentials.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::ErrorOr;
  using Utils::RefPtr;
  using Utils::StringView;

  class VirtualFileSystem {
  private:
    RefPtr<Inode> m_root_inode;

  public:
    VirtualFileSystem() = default;
    static void init();
    static VirtualFileSystem& the();
    ~VirtualFileSystem() = default;

    ErrorOr<RefPtr<FileSystem>> device_load_filesystem(RefPtr<Device> device);

    ErrorOr<void> mount(const StringView& path, RefPtr<Inode> inode);
    ErrorOr<void> unmount(const StringView& path);
    ErrorOr<RefPtr<Inode>> open(const Credentials& credentials, const String& path, FileOpenMode mode);
    ErrorOr<void> close(RefPtr<Inode> inode);
  };
}// namespace Kernel