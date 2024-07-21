#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Security/Credentials.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::ErrorOr;
  using Utils::RefPtr;
  using Utils::StringView;

  using FileOpenMode = size_t;

  enum class FileAccessMode : u8 {
    Read = 0,
    Write,
    ReadWrite,
  };

  template <typename... Modes>
  constexpr FileOpenMode build_file_open_mode(Modes... modes) {
    return (static_cast<FileOpenMode>(modes) | ...);
  }

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
    ErrorOr<RefPtr<Inode>> open(const Credentials& credentials, const StringView& path, FileOpenMode mode);
    ErrorOr<void> close(RefPtr<Inode> inode);
  };
}// namespace Kernel