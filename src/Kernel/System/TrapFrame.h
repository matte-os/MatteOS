#pragma once

#include "Kernel/Satp.hh"
#include "Utils/Types.hh"

namespace Kernel {
    struct TrapFrame{
        u64 regs[32];
        u64 fregs[32];
        SATP satp;
        u64* trap_stack;
        u64 hart_id;
    };
};