#include <Kernel/CPU.h>
#include <Utils/Assertions.hh>
#include <Utils/DebugConsole.hh>

using Kernel::CPU;

namespace Utils{
    void ASSERT(bool condition){
        if(!condition){
            DebugConsole::println("Assetion");
            CPU::halt();
        }
    }
};