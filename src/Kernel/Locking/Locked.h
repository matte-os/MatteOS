#pragma once

#include <Utils/Traits.h>

namespace Kernel {
  using Utils::same_as;
  template<typename T>
  concept Lockable = requires(T t) {
    { t.lock() } -> same_as<void>;
    { t.unlock() } -> same_as<void>;
  };

  template<Lockable T>
  class Locked {
    
  };
}


