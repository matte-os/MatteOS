#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::RefCounted;

  class FileDescriptor : public RefCounted<FileDescriptor> {
  public:
    FileDescriptor() = default;
    virtual ~FileDescriptor() = default;

    virtual bool can_read() const = 0;
    virtual bool can_write() const = 0;

    virtual size_t read(u8* buffer, size_t size, size_t offset) = 0;
    virtual size_t write(const u8* buffer, size_t size, size_t offset) = 0;

    virtual void close() = 0;
  };
}// namespace Kernel