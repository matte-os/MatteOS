#include <Kernel/CPU.hh>

namespace Kernel
{
    TrapFrame CPU::KERNEL_TRAP_FRAME[8] = {};

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

    void CPU::mscratchWrite(u64 value){
        asm volatile("csrw mscratch, %0" ::"r"(value));
    }

    u64 CPU::mscratchRead(){
        u64 rval;
        asm volatile("csrr %0, mscratch" : "=r"(rval));
        return rval;
    }

    void CPU::sscratchWrite(u64 value){
        asm volatile("csrw sscratch, %0" ::"r"(value));
    }

    u64 CPU::sscratchRead(){
        u64 rval;
        asm volatile("csrr %0, sscratch" : "=r"(rval));
        return rval;
    }

    void CPU::satpFence(u64 vaddr, u64 asid){
        asm volatile("sfence.vma %0, %1" :: "r"(vaddr), "r"(asid));
    }

    void CPU::satpFenceAsid(u64 asid){
        asm volatile("sfence.vma zero, %0" :: "r"(asid));
    }
};
