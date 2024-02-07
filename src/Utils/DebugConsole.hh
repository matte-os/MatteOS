#pragma once

#include <Utils/Types.hh>

namespace Utils{
    class DebugConsole{
        public:
            //static void dprintf(const char *, ...);
            static void print(const char);
            static void print(const char *);
            static void println(const char *);
            static void dbgln(const char *, ...);
            static void print_number(u64 number, u32 base);
            static void print_ln_number(u64 number, u32 base);
    };
};