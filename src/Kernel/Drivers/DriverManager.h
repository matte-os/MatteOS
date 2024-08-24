//
// Created by matejbucek on 6.7.24.
//

#pragma once

#include <Kernel/Drivers/Driver.h>
#include <Kernel/Forwards.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {

  using Utils::ArrayList;
  using Utils::ErrorOr;

  class DriverManager {
  private:
    ArrayList<RefPtr<DriverDescriptor>> m_driver_descriptors;

  public:
    static DriverManager& the();
    static void init();
    ArrayList<RefPtr<DriverDescriptor>> find_compatible_drivers(RefPtr<Device> device);

  private:
    DriverManager() = default;
    void register_default_drivers();
    void register_driver(RefPtr<DriverDescriptor> driver_descriptor);
  };
}// namespace Kernel