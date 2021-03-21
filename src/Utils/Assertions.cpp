#include <Utils/Assertions.hh>
#include <Utils/DebugConsole.hh>
#include <Kernel/CPU.hh>

using Kernel::CPU;

namespace Utils{
    void ASSERT(bool condition){
        if(!condition){
            DebugConsole::println("Assetion");
            CPU::halt();
        }
    }
};