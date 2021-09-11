#pragma once

#include <Utils/Types.hh>
#include <Kernel/Satp.hh>

namespace Kernel{
    enum class SatpMode : u8{
        OFF = 0,
        Sv39 = 8,
        Sv48 = 9
    };

    class CPU{
    public:
        static void halt();
        static SATP buildSatp(SatpMode, u64, u64);
        static u64 readSatp();
        static void writeSatp(u64);
    };
};