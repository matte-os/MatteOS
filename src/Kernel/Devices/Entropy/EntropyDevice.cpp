/**
 * @file EntropyDevice.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Devices/Entropy/EntropyDevice.h>

namespace Kernel {
  ErrorOr<void> EntropyDevice::init() {
    return m_underlying_device->as<VirtIODevice>()->init(0, 1, [](VirtQueue*, u64 queue_id) {
      DebugConsole::println("EntropyDevice: Initialising VirtQueue.");
    });
  }
}// namespace Kernel