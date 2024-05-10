#pragma once

#include <Kernel/Satp.h>
#include <Kernel/System/TrapFrame.h>
#include <Utils/Types.hh>

namespace Kernel{
    enum class SatpMode : u8{
        OFF = 0,
        Sv39 = 8,
        Sv48 = 9
    };

    class CPU{
    public:
        static TrapFrame KERNEL_TRAP_FRAME[8];
        static void halt();
        static SATP build_satp(SatpMode, u64, u64);
        static SATP read_satp();
        static void write_satp(u64);
        static void mscratch_write(u64);
        static u64 mscratch_read();
        static void sscratch_write(u64);
        static u64 sscratch_read();
        static void satp_fence(u64, u64);
        static void satp_fence_asid(u64);
        static void write_sstatus(u64);
        static void write_stvec(void (*trap_vector)());
        static void write_sie(u64 sie);
    };
};
