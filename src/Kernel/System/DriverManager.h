//
// Created by matejbucek on 6.7.24.
//

#pragma once

#include <Utils/Pointers/RefCounted.h>

namespace Kernel {
  class Driver : public RefCounted<Driver> {
  public:
    virtual void init() = 0;
    virtual void shutdown() = 0;
  };

  class DriverManager {
  public:
    static DriverManager& the();
    static void init();
  };
}// namespace Kernel