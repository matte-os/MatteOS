#pragma once

#include <Utils/Types.hh>
#include <Utils/Optional.hh>

using Utils::Optional;

namespace Kernel{
    class Plic{
    public:
        static const size_t PLIC_PRIORITY = 0x0c000000;
        static const size_t PLIC_PENDING = 0x0c001000;
        static const size_t PLIC_INT_ENABLE = 0x0c002000;
        static const size_t PLIC_TRESHOLD = 0x0c200000;
        static const size_t PLIC_CLAIM = 0x0c200004;
        static void enable(u32);
        static void setPriority(u32, u8);
        static void setTreshold(u8);
        static Optional<u32> next();
        static void complete(u32);
        static bool isPending(u32);
    };
};
