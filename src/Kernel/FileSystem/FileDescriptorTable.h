#pragma once

#include <Kernel/FileSystem/FileDescriptor.h>
#include <Kernel/FileSystem/FileOpenMode.h>
#include <Kernel/FileSystem/Inode.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Pointers/RefPtr.h>

namespace Kernel {
  using Utils::Array;

  class FileDescriptorTable {
  private:
    static const size_t MAX_FDS = 256;
    Array<RefPtr<FileDescriptor>> m_fds;

  public:
    FileDescriptorTable() : m_fds(MAX_FDS) {};
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
