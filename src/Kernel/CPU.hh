#pragma once

#include <Utils/Types.hh>
#include <Kernel/Satp.hh>
#include "Kernel/System/TrapFrame.hh"

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
        static SATP buildSatp(SatpMode, u64, u64);
        static u64 readSatp();
        static void writeSatp(u64);
        static void mscratchWrite(u64);
        static u64 mscratchRead();
        static void sscratchWrite(u64);
        static u64 sscratchRead();
        static void satpFence(u64, u64);
        static void satpFenceAsid(u64);
    };
};