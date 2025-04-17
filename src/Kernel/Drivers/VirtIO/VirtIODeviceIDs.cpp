/**
 * @file VirtIODeviceIDs.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Drivers/VirtIO/VirtIODeviceIDs.h>

namespace Kernel {
  VirtIODeviceIDs get_device_type(u32 device_id) {
    return static_cast<VirtIODeviceIDs>(device_id);
  }
}// namespace Kernel
