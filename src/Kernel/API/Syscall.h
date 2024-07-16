#pragma once

#include <Utils/Errors/ErrorOr.h>

enum class SysError {};

using SysResult = Utils::ErrorOr<uintptr_t , SysError>;

typedef SysResult (*SyscallHandler)();

#define SYSCALL_HANDLER(name) \
    SysResult handle_##name();

#define SYSCALL_HANDLER_FUN(name) handle_##name,

#define SYSCALL_ENUM(name) Sys$##name,
#define SYSCALL_NAME(name) #name,

#define SYSCALL_GENERATOR(F) \
    F(open) \
    F(close)

namespace Kernel {
  enum class Syscalls : size_t {
    SYSCALL_GENERATOR(SYSCALL_ENUM)
    __Count
  };

  static const char* syscall_names[] = {
    SYSCALL_GENERATOR(SYSCALL_NAME)
  };

  SYSCALL_GENERATOR(SYSCALL_HANDLER)
}