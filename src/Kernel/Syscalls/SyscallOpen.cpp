#include <Kernel/API/Syscall.h>

namespace Kernel {
  SysResult handle_open() {
    return SysResult::create(0);
  }

  SysResult handle_close() {
    return SysResult::create(0);
  }
}// namespace Kernel