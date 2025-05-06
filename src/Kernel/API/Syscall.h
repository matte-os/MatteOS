/**
 * @file Syscall.h
 * @author Matěj Bucek
 *
 * This code was inspired by SerenityOS's implementation.
 */

#pragma once

#include <Utils/Errors/ErrorOr.h>

#define SYSCALL_ENUM(name) Sys$##name,
#define SYSCALL_NAME(name) #name,

#define SYSCALL_GENERATOR(F) \
  F(open)                    \
  F(close)                   \
  F(dbgln)                   \
  F(read)                    \
  F(write)                   \
  F(exit)                    \
  F(dmesg)                   \
  F(stats)                   \
  F(mem)                     \
  F(ls)

namespace Kernel {
  enum class SysError : size_t {
    Error,
    Blocked
  };

  using ErrorOrSyscall = Utils::ErrorOr<uintptr_t, SysError>;

  enum class Syscalls : size_t {
    SYSCALL_GENERATOR(SYSCALL_ENUM)
            __Count
  };

  static const char* syscall_names[] = {
          SYSCALL_GENERATOR(SYSCALL_NAME)};

  inline uintptr_t syscall(Syscalls syscall) {
    register uintptr_t a7 asm("a7") = Utils::as_underlying(syscall);
    register uintptr_t result asm("a0");

    asm volatile("ecall"
                 : "=r"(result)
                 : "r"(a7));
    return result;
  }

  template<typename T1>
  uintptr_t syscall(Syscalls syscall, T1 arg1) {
    register uintptr_t a7 asm("a7") = static_cast<size_t>(syscall);
    register uintptr_t a0 asm("a0") = static_cast<uintptr_t>(arg1);
    register uintptr_t result asm("a0");

    asm volatile("ecall"
                 : "=r"(result)
                 : "r"(a0), "r"(a7)
                 : "memory");
    return result;
  }

  template<typename T1, typename T2>
  uintptr_t syscall(Syscalls syscall, T1 arg1, T2 arg2) {
    register uintptr_t a7 asm("a7") = static_cast<size_t>(syscall);
    register uintptr_t a0 asm("a0") = static_cast<uintptr_t>(arg1);
    register uintptr_t a1 asm("a1") = static_cast<uintptr_t>(arg2);
    register uintptr_t result asm("a0");

    asm volatile("ecall"
                 : "=r"(result)
                 : "r"(a0), "r"(a1), "r"(a7)
                 : "memory");
    return result;
  }

  template<typename T1, typename T2, typename T3>
  uintptr_t syscall(Syscalls syscall, T1 arg1, T2 arg2, T3 arg3) {
    register uintptr_t a7 asm("a7") = static_cast<size_t>(syscall);
    register uintptr_t a0 asm("a0") = static_cast<uintptr_t>(arg1);
    register uintptr_t a1 asm("a1") = static_cast<uintptr_t>(arg2);
    register uintptr_t a2 asm("a2") = static_cast<uintptr_t>(arg3);

    uintptr_t value;
    asm volatile("ecall"
                 : "=r"(value)
                 : "r"(a0), "r"(a1), "r"(a2), "r"(a7)
                 : "memory");
    return value;
  }

  template<typename T1, typename T2, typename T3, typename T4>
  uintptr_t syscall(Syscalls syscall, T1 arg1, T2 arg2, T3 arg3, T4 arg4) {
    register uintptr_t a7 asm("a7") = static_cast<size_t>(syscall);
    register uintptr_t a0 asm("a0") = static_cast<uintptr_t>(arg1);
    register uintptr_t a1 asm("a1") = static_cast<uintptr_t>(arg2);
    register uintptr_t a2 asm("a2") = static_cast<uintptr_t>(arg3);
    register uintptr_t a3 asm("a3") = static_cast<uintptr_t>(arg4);

    uintptr_t value;
    asm volatile("ecall"
                 : "=r"(value)
                 : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a7)
                 : "memory");
    return value;
  }

}// namespace Kernel