/**
 * @file File.cpp
 * @author Matěj Bucek
 */
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/FileSystem/File.h>

namespace Kernel {
  bool File::can_read() const {
    return false;
  }
  bool File::can_write() const {
    return false;
  }
  ErrorOr<Request<size_t>> File::read(u8* buffer, size_t size) {
    auto request = m_open_file_descriptor->inode()->read(buffer, size, m_offset);
    if(request.has_value()) {
      auto value = request.get_value();
      if(value.is_finalized()) {
        m_offset += value.release_value();
      }
    }
    return request;
  }
  size_t File::write(const u8* buffer, size_t size, size_t offset) {
    return 0;
  }
  void File::close() {
    auto& vfs = VirtualFileSystem::the();
    //vfs.close(m_inode);
    //m_inode = {};
  }
}// namespace Kernel