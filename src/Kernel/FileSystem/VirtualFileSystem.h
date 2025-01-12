#pragma once

#include <Kernel/FileSystem/FileOpenMode.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Security/Credentials.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>
#include <Utils/Pair.h>

namespace Kernel {
  using Utils::ErrorOr;
  using Utils::HashMap;
  using Utils::RefPtr;
  using Utils::String;
  using Utils::StringView;
  using Utils::Pair;
  using Utils::ArrayList;

  class MountFlags {
  public:
    static constexpr u64 implicit() {
      return 0;
    }
  };

  class MountContext : public RefCounted<MountContext> {
    RefPtr<FileSystem> m_file_system;
    u64 m_mount_flags;

  public:
    MountContext() = default;

    explicit MountContext(RefPtr<FileSystem> file_system, u64 mount_flags) : m_file_system(move(file_system)), m_mount_flags(mount_flags) {}

    const RefPtr<FileSystem>& fs() const {
      return m_file_system;
    }
  };

  class VirtualFileSystem {
    RefPtr<Inode> m_root_inode;
    HashMap<String, RefPtr<MountContext>> m_mounts;

    VirtualFileSystem() = default;
    ErrorOr<Pair<RefPtr<FileSystem>, u32>> longest_common_prefix(const ArrayList<String>& path);

  public:
    static void init();
    static VirtualFileSystem& the();
    ~VirtualFileSystem() = default;

    ErrorOr<RefPtr<FileSystem>> device_load_filesystem(RefPtr<Device> device);

    ErrorOr<void> mount(const StringView& mount_point, const StringView& filesystem_path);
    ErrorOr<void> mount(const StringView& mount_point, const RefPtr<FileSystem>& file_system);
    ErrorOr<void> unmount(const StringView& mount_point);
    ErrorOr<RefPtr<OpenFileDescriptor>> open(const Credentials& credentials, const String& path, FileOpenMode mode);
    ErrorOr<void> close(RefPtr<OpenFileDescriptor> file);
    ErrorOr<void> mount_root_fs(RefPtr<FileSystem> file_system);

  private:
    ErrorOr<void> fs_mount(const StringView& mount_point, RefPtr<FileSystem> file_system, u64 mount_flags);
  };
}// namespace Kernel