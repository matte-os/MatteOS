//
// Created by matejbucek on 21.10.22.
//

#pragma once
#include <Kernel/System/TrapFrame.h>
#include "Process.h"

namespace Kernel::Process {
    class Scheduler {
    public:
        static void init();
        static Scheduler& the();
        Process* schedule();
        [[noreturn]] void start_scheduling();
    private:
        Scheduler() = default;
    };
} // Kernel
