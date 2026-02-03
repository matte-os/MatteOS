/**
 * @file TrapFrame.h
 * @author Matěj Bucek
 *
 * This file was inspired by OSBlog.
 */
#pragma once

#include <Kernel/Arch/RegisterOffset.h>
#include <Kernel/Arch/riscv64/Satp.h>
#include <Utils/Types.h>

namespace Kernel {
  struct TrapFrame {
    u64 regs[32];
    u64 fregs[32];
    u64 sepc;   // 512
    u64 sstatus;// 520
    u64 scause; // 528
    u64 stval;  // 536

    u64 kernel_satp;          // User/Kernel SATP, dual use
    u64 kernel_sp;            // The Kernel Stack Pointer for this thread
    u64 kernel_trap_frame_ptr;// The pointer to the Kernel Thread's context (For 'tp')

    u64 kernel_addr;

    template<typename T>
    T get_register(RegisterOffset offset) {
      return reinterpret_cast<T>(regs[static_cast<u64>(offset)]);
    }

    template<typename T>
    void set_register(RegisterOffset offset, T value) {
      regs[static_cast<u64>(offset)] = reinterpret_cast<u64>(value);
    }
  };


};// namespace Kernel
