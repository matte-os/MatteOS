#include <Kernel/Process/Console/Console.h>
#include <Kernel/API/Syscall.h>
#include <Utils/Types.h>

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
