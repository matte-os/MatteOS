#include <Kernel/Process/Console/Console.h>
#include <Kernel/API/Syscall.h>
#include <Utils/Types.h>

#if 0
extern "C"
int console_main() {
    int counter = 0;
    while (true) {
        if (counter == 1000000000) {
            Kernel::syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Process 2 is alive!"));
            counter = 0;
            continue;
        }

        counter++;
    }
}
#endif

extern "C"
int console_main() {
  auto fd = static_cast<int>(syscall(Kernel::Syscalls::Sys$open, reinterpret_cast<uintptr_t>("/dev/tty0"), 0));
  if(fd < 0) {
    syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Couldn't open the tty."));
    while(true) {}
  }

  char buffer[512] = {};
  while(true) {
    if(syscall(Kernel::Syscalls::Sys$read, fd, reinterpret_cast<uintptr_t>(buffer), sizeof(buffer)) > 0) {
      syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Got message"));
      syscall(Kernel::Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>(buffer));
    }
  }

  Kernel::syscall(Kernel::Syscalls::Sys$exit, 0);
}