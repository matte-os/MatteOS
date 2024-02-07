#include <Utils/DebugConsole.hh>
#include <Kernel/Uart.hh>
#include <Utils/Basic.hh>
#include <Kernel/sbi/sbi.hh>
#include <stdarg.h>

using Kernel::Uart;

namespace Utils{
    void DebugConsole::print(const char c){
        Kernel::SBI::sbi_console_putchar(c);
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

    void DebugConsole::print_number(u64 number, u32 base){
        char buffer[64];
        itoa(buffer, number, base);
        print(buffer);
    }

    void DebugConsole::print_ln_number(u64 number, u32 base){
        print_number(number, base);
        print('\n');
    }

    void DebugConsole::dbgln(const char * fmt, ...) {
/*        va_list args;
        va_start(args, fmt);

        va_end(args);*/
    }
};