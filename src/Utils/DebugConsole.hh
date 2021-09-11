#pragma once

#include <Utils/Types.hh>

namespace Utils{
    class DebugConsole{
        public:
            //static void dprintf(const char *, ...);
            static void print(const char);
            static void print(const char *);
            static void println(const char *);
            static void printNumber(u64 number, u32 base);
            static void printLnNumber(u64 number, u32 base);
    };
};