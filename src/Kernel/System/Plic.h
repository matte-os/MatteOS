#pragma once

#include <Utils/Optional.h>
#include <Utils/Types.h>

using Utils::Optional;

namespace Kernel::System {
    class Plic{
    private:
        size_t PLIC_PRIORITY;
        size_t PLIC_PENDING;
        size_t PLIC_INT_ENABLE;
        size_t PLIC_TRESHOLD;
        size_t PLIC_CLAIM;
    public:
        static Plic& the();
        void init(size_t, size_t, size_t, size_t, size_t);
        void enable(u32);
        void set_priority(u32 id, u8 priority);
        void set_treshold(u8 treshold);
        Optional<u32> next();
        void complete(u32);
        bool isPending(u32);
    private:
        Plic() : Plic(0x0c000000, 0x0c001000, 0x0c002000, 0x0c200000, 0x0c200004) {};
        Plic(size_t plic_priority, size_t plic_pending, size_t plic_int_enable, size_t plic_treshold, size_t plic_claim) : PLIC_PRIORITY(plic_priority), PLIC_PENDING(plic_pending), PLIC_INT_ENABLE(plic_int_enable), PLIC_TRESHOLD(plic_treshold), PLIC_CLAIM(plic_claim) {};
        ~Plic() = default;
    };
};
