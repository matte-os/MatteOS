/**
 * @file DriverManager.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/Drivers/Driver.h>
#include <Kernel/Forwards.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {

  using Utils::ArrayList;
  using Utils::ErrorOr;

  class DriverManager {
  private:
    ArrayList<RefPtr<DeviceDriverDescriptor>> m_device_driver_descriptors;
    ArrayList<RefPtr<DriverDescriptor>> m_driver_descriptors;
    ArrayList<RefPtr<Driver>> m_drivers;

  public:
    static DriverManager& the();
    static void init();
    ArrayList<RefPtr<DeviceDriverDescriptor>> find_compatible_drivers(RefPtr<Device> device);
    void init_drivers();

  private:
    DriverManager() = default;
    void register_default_drivers();
    void register_device_driver(RefPtr<DeviceDriverDescriptor> driver_descriptor);
    void register_driver(RefPtr<DriverDescriptor> driver_descriptor);
  };
}// namespace Kernel