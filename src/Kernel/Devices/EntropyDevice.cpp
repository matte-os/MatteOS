//
// Created by matejbucek on 3.9.24.
//

#include <Kernel/Devices/EntropyDevice.h>

namespace Kernel {
  ErrorOr<void> EntropyDevice::init() {
    return m_underlying_device->as<VirtIODevice>()->init(0, 1, [](VirtQueue*, u64 queue_id) {
      DebugConsole::println("EntropyDevice: Initialising VirtQueue.");
    });
  }
}// namespace Kernel