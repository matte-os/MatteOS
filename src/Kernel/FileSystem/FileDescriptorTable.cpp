#include <Kernel/FileSystem/FileDescriptorTable.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  using Utils::Error;

  ErrorOr<size_t> FileDescriptorTable::open(const RefPtr<FileDescriptor>& descriptor) {
    for(size_t i = 0; i < m_fds.size(); i++) {
      if(!m_fds[i]) {
        m_fds[i] = descriptor;
        return ErrorOr<size_t>::create(i);
      }
    }

    return ErrorOr<size_t>::create_error(Error::create_from_string("No free file descriptors!"));
  }
  ErrorOr<void> FileDescriptorTable::close(size_t fd) {
    if(fd >= m_fds.size()) {
      return ErrorOr<void>::create_error(Error::create_from_string("Invalid file descriptor!"));
    }

    if(!m_fds[fd]) {
      return ErrorOr<void>::create_error(Error::create_from_string("File descriptor already closed!"));
    }

    m_fds[fd] = {};
    return ErrorOr<void>::create({});
  }
  ErrorOr<void> FileDescriptorTable::close_all() {
    for(size_t i = 0; i < m_fds.size(); i++) {
      if(m_fds[i]) {
        m_fds[i] = RefPtr<FileDescriptor>();
      }
    }

    return ErrorOr<void>::create({});
  }
  ErrorOr<RefPtr<FileDescriptor>> FileDescriptorTable::get(size_t fd) {
    if(!m_fds[fd]) {
      return ErrorOr<RefPtr<FileDescriptor>>::create_error(Error::create_from_string("Invalid file descriptor!"));
    }
    return ErrorOr<RefPtr<FileDescriptor>>::create(m_fds[fd]);
  }
}// namespace Kernel
