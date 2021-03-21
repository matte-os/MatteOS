#pragma once

namespace Utils{
    class DebugConsole{
        public:
            //static void dprintf(const char *, ...);
            static void print(const char);
            static void println(const char *);
    };
};