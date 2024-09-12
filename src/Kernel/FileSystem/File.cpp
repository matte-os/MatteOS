#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/FileSystem/File.h>

namespace Kernel {
  bool File::can_read() const {
    return false;
  }
  bool File::can_write() const {
    return false;
  }
  size_t File::read(u8* buffer, size_t size, size_t offset) {
    return 0;
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