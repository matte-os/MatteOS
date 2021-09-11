#include <Kernel/CPU.hh>

namespace Kernel
{
    void CPU::halt(){
        for(;;){}
    }  

    SATP CPU::buildSatp(SatpMode mode, u64 asid, u64 addr){
        SATP satp{.mode = (u8)mode, .asid = asid, .ppn = (addr >> 12)};
        return satp;
    }

    void CPU::writeSatp(u64 value){
        asm volatile("csrw satp, %0;" :: "r"(value));
    }

    u64 CPU::readSatp(){
        size_t rval;
        asm volatile("csrr %0, satp;" : "=r"(rval));
        return rval;
    }
};
