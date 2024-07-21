#pragma once

#include <Utils/Errors/ErrorOr.h>

#define SYSCALL_ENUM(name) Sys$##name,
#define SYSCALL_NAME(name) #name,

#define SYSCALL_GENERATOR(F) \
  F(open)                    \
  F(close)

namespace Kernel {
  enum class SysError {};

  enum class Syscalls : size_t {
    SYSCALL_GENERATOR(SYSCALL_ENUM)
            __Count
  };

  static const char* syscall_names[] = {
          SYSCALL_GENERATOR(SYSCALL_NAME)};

}// namespace Kernel