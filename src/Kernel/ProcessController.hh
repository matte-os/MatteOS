#pragma once

#include <Kernel/Process.hh>
#include <Utils/Types.hh>

namespace Kernel{
    class ProcessController
    {
    private:
        static const u64 STACK_PAGES = 2;
        static const u64 STACK_ADDRESS = 0xf00000000;
        static const u64 STARTING_ADDRESS = 0x20000000;
        u64 nextPid;
        Process processList[32];
    public:
        ProcessController();
        ~ProcessController();
        Process initProcess(uintptr_t);
    };
};