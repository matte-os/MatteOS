#pragma once

#include <Utils/Types.h>

namespace Kernel {
  struct SATP {
    u64 ppn  : 44;//First PPN of PageTable
    u64 asid : 16;//SATP ASID for TLB
    u64 mode : 4; //SATP Mode
  };
};// namespace Kernel