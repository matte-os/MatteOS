#include <Kernel/Devices/Device.h>
#include <Kernel/FileSystem/FATFS/FATFileSystem.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Assertions.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::Error;
using Utils::ErrorOr;

namespace Kernel {
  static VirtualFileSystem* s_instance = nullptr;

  void VirtualFileSystem::init() {
    if(!s_instance) {
      s_instance = new VirtualFileSystem();
    }
  }

  VirtualFileSystem& VirtualFileSystem::the() {
    runtime_assert(s_instance, "VirtualFileSystem not initialized");
    return *s_instance;
  }

  ErrorOr<RefPtr<OpenFileDescriptor>> VirtualFileSystem::open(const Credentials& credentials, const String& path, FileOpenMode mode) {
    auto path_parts = path.split("/");

    //This traverses all the mount points and finds the one with the longest common prefix
    auto error_or_mount = longest_common_prefix(path_parts);
    if(error_or_mount.has_error()) {
      DebugConsole::println("Error in longest common prefix");
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_mount.get_error());
    }

    auto mount = error_or_mount.get_value();
    auto& fs = mount.first;
    auto longest = mount.second;

    String relative_path = "/";
    for(u32 i = longest; i < path_parts.size(); i++) {
      relative_path += path_parts[i];
      if(i < path_parts.size() - 1) {
        relative_path += "/";
      }
    }

    if(!fs) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("No filesystem mounted at this path!"));
    }

    if(!fs->exists(relative_path)) {
      if(mode.is<FileOperationMode::Create>()) {
        //TODO: Implement create
      }
    } else {
      if(mode.is<FileOperationMode::Exclusive>()) {
        return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Cannot open file in exclusive mode! File already exists!"));
      }
    }

    auto error_or_open = fs->open(credentials, relative_path, mode);

    if(error_or_open.has_error()) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_open.get_error());
    }

    return ErrorOr<RefPtr<OpenFileDescriptor>>::create(error_or_open.get_value());
  }

  ErrorOr<void> VirtualFileSystem::close(RefPtr<OpenFileDescriptor> file) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented"));
  }

  ErrorOr<RefPtr<FileSystem>> VirtualFileSystem::device_load_filesystem(RefPtr<Device> device) {
    if(device->get_device_type() != DeviceType::Block) {
      return ErrorOr<RefPtr<FileSystem>>::create_error(Error::create_from_string("Device is not a block device!"));
    }

    return FATFileSystem::try_create(device);
  }

  ErrorOr<void> VirtualFileSystem::mount(const StringView& mount_point, const StringView& filesystem_path) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented"));
  }

  ErrorOr<void> VirtualFileSystem::unmount(const StringView& mount_point) {
    return ErrorOr<void>::create_error(Error::create_from_string("Not implemented"));
  }

  ErrorOr<void> VirtualFileSystem::fs_mount(const StringView& mount_point, RefPtr<FileSystem> file_system, u64 mount_flags) {
    if(m_mounts.has_key(mount_point)) {
      return ErrorOr<void>::create_error(Error::create_from_string("A filesystem already mounted here!"));
    }

    m_mounts.set(mount_point, RefPtr<MountContext>(new MountContext(move(file_system), mount_flags)));
    return ErrorOr<void>::create({});
  }

  ErrorOr<void> VirtualFileSystem::mount_root_fs(RefPtr<FileSystem> file_system) {
    auto error_or_fs = file_system->root();
    if(error_or_fs.has_error()) {
      return ErrorOr<void>::create_error(error_or_fs.get_error());
    }

    m_root_inode = move(error_or_fs.get_value());
    fs_mount("/", file_system, MountFlags::implicit());
    return ErrorOr<void>::create({});
  }

  ErrorOr<Pair<RefPtr<FileSystem>, u32>> VirtualFileSystem::longest_common_prefix(const ArrayList<String>& path) {
    if(!m_root_inode) {
      return ErrorOr<Pair<RefPtr<FileSystem>, u32>>::create_error(Error::create_from_string("No root filesystem mounted!"));
    }

    RefPtr<FileSystem> fs = m_root_inode->fs();
    u32 longest = 0;
    m_mounts.for_each([&](const String& mount_point, const RefPtr<MountContext>& context) {
      auto mount_parts = mount_point.split("/");
      for(u32 i = 0; i < path.size() && i < mount_parts.size(); i++) {
        if(path[i] != mount_parts[i]) {
          break;
        }
        if(i > longest) {
          longest = i;
          fs = context->fs();
        }
      }
    });

    if(!fs) {
      return ErrorOr<Pair<RefPtr<FileSystem>, u32>>::create_error(Error::create_from_string("No filesystem mounted at this path!"));
    }

    return ErrorOr<Pair<RefPtr<FileSystem>, u32>>::create({fs, longest});
  }
}// namespace Kernel