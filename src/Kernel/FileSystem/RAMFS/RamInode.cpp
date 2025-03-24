#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/Device.h>
#include <Kernel/FileSystem/RAMFS/RamFileSystem.h>
#include <Kernel/FileSystem/RAMFS/RamInode.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Math.h>
#include <Kernel/Process/Request.h>

namespace Kernel {
  using Utils::Error;

  RefPtr<FileSystem> RamInode::fs() {
    return static_cast<RefPtr<FileSystem>>(m_fs);
  }

  ErrorOr<void> RamInode::check_credentials(const Credentials& credentials, const FileOpenMode& mode) {
    return {};
  }

  ErrorOr<RefPtr<Inode>> RamInode::get_child(StringView name) {
    return TRY(m_children.get(name));
  }

  ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> RamInode::list_dir() {
    ArrayList<RefPtr<DirectoryEntry>> entries;
    m_children.for_each([&](const auto& name, RefPtr<Inode> inode) {
      entries.add(RefPtr(new DirectoryEntry(name)));
    });

    return entries;
  }

  ErrorOr<StringView> RamInode::name() {
    return StringView("");
  }

  bool RamInode::is_directory() {
    return m_type == Type::RamDirectory;
  }

  bool RamInode::is_file() {
    return m_type == Type::RamFile;
  }

  RefPtr<RamInode> RamInode::create_directory() {
    return {new RamInode(Type::RamDirectory)};
  }

  ErrorOr<Request<size_t>> RamInode::read(u8* buffer, size_t size, size_t offset) {
    return Error::create_from_string("Not implemented!");
  }

  ErrorOr<size_t> RamInode::write(const u8* buffer, size_t size, size_t offset) {
    return ErrorOr<size_t>::create_error(Error::create_from_string("Not implemented!"));
  }

  bool DevInode::is_directory() {
    return false;
  }

  bool DevInode::is_file() {
    return true;
  }

  ErrorOr<Request<size_t>> DevInode::read(u8* buffer, size_t size, size_t offset) {
    if(m_device->get_device_type() == DeviceType::Console) {
      auto console = m_device->as<ConsoleDevice>();
      auto device_request = TRY(console->read_async());

      if(device_request.is_blocked()) {
        return Request<size_t>::propagate_blocked(device_request);
      }

      const auto to_copy = Utils::min(size, device_request.release_value().length());

      Utils::memcpy(reinterpret_cast<char*>(buffer), device_request.release_value().to_cstring(), to_copy);
      return Request<size_t>::create_finalized(to_copy);
    }
    return Error::create_from_string("Not implemented!");
  }
}// namespace Kernel