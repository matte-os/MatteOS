#pragma once

#include <Utils/Types.h>

extern "C" {
  struct KernelSymbol {
    const char* name;
    uintptr_t address;
  };

  extern const KernelSymbol k_exported_symbols[];
  extern const size_t k_exported_symbols_count;
}