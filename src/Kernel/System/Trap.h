#pragma once

#include "TrapFrame.h"
#include "Utils/Types.h"

using Kernel::TrapFrame;

extern "C" size_t trap_vector(
        u64 sepc,
        u64 stval,
        u64 scause,
        u64 cpu_id,
        u64 sstatus);