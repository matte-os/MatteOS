#pragma once

#include <Kernel/Devices/Device.h>
#include <Kernel/Process/Request.h>

namespace Kernel {

  class ConsoleDevice : public Device {
    ArrayList<RefPtr<RequestContext>> m_requests;
  public:
    explicit ConsoleDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Console, true) {}

    ErrorOr<void> init() override;
    ErrorOr<void> write(char c);
    ErrorOr<void> write(String message);
    ErrorOr<String> read();
    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;

    ErrorOr<Request<String>> read_async();
  };

}// namespace Kernel
