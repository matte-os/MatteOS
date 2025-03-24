#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Process/Request.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::RefCounted;

  class FileDescriptor : public RefCounted<FileDescriptor> {
  public:
    FileDescriptor() = default;
    virtual ~FileDescriptor() = default;

    virtual bool can_read() const = 0;
    virtual bool can_write() const = 0;

    virtual ErrorOr<Request<size_t>> read(u8* buffer, size_t size) = 0;
    virtual size_t write(const u8* buffer, size_t size, size_t offset) = 0;

    virtual void close() = 0;
  };

  using Utils::Array;

  class FileDescriptorTable {
    static const size_t MAX_FDS = 256;
    Array<RefPtr<FileDescriptor>> m_fds;

  public:
    FileDescriptorTable() : m_fds(MAX_FDS) {
                            };
    virtual ~FileDescriptorTable() = default;
    ErrorOr<size_t> open(const RefPtr<FileDescriptor>& descriptor);
    ErrorOr<RefPtr<FileDescriptor>> get(size_t fd);
    ErrorOr<void> close(size_t fd);
    ErrorOr<void> close_all();

    RefPtr<FileDescriptor> operator[](size_t fd) {
      auto descriptor_or_error = get(fd);
      if(descriptor_or_error.has_error()) {
        return {};
      } else {
        return descriptor_or_error.get_value();
      }
    }
  };
}// namespace Kernel
