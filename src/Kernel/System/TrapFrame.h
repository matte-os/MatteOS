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
    uintptr_t program_counter;
    SATP satp;
    u64* trap_stack;

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
