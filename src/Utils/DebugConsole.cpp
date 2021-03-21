#include <Utils/DebugConsole.hh>
#include <Kernel/Uart.hh>

using Kernel::Uart;

namespace Utils{
    void DebugConsole::print(const char c){
        Uart uart(0x10000000); 
        uart.put(c);
    }

    void DebugConsole::println(const char * str){
        for (size_t i = 0; 1; i++)
        {
            char character = str[i];
            if(character == '\0'){
                print('\n');
                return;
            }
            print(character);
        }
    }
};