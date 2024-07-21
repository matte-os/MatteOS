#pragma once

#include <Utils/Types.h>

namespace Kernel {
  struct Credentials {
    size_t uid;
    size_t gid;
    size_t euid;
    size_t egid;
    size_t suid;
    size_t sgid;
    size_t fsuid;
    size_t fsgid;
  };
}