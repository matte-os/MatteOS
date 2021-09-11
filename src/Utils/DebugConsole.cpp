#include <Utils/DebugConsole.hh>
#include <Kernel/Uart.hh>
#include <Utils/Basic.hh>

using Kernel::Uart;

namespace Utils{
    void DebugConsole::print(const char c){
        Uart uart(0x10000000); 
        uart.put(c);
    }

    void DebugConsole::print(const char * str){
        for (size_t i = 0; 1; i++)
        {
            char character = str[i];
            if(character == '\0'){
                return;
            }
            print(character);
        }
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

    void DebugConsole::printNumber(u64 number, u32 base){
        char buffer[64];
        itoa(buffer, number, base);
        print(buffer);
    }

    void DebugConsole::printLnNumber(u64 number, u32 base){
        printNumber(number, base);
        print('\n');
    }
};