#include <Kernel/Devices/Device.h>
#include <Kernel/Drivers/Console/NS16550ADriver.h>

namespace Kernel {
  void NS16550ADriver::init(RefPtr<Device> device) {
    m_device = device->as<ConsoleDevice>();

    // Initialize the UART.
    auto underlying_device = m_device->get_underlying_device()->as<UniversalDevice>();
    auto address = reinterpret_cast<u8*>(underlying_device->get_address());

    /*
     * Basically, the NS16550A UART has the following registers:
     * 0x00: Data register (R/W)
     * 0x01: Interrupt enable register (R/W)
     * 0x02: FIFO control register (W)
     * 0x03: Line control register (R/W)
     * 0x04: Modem control register (R/W)
     * 0x05: Line status register (R)
     * 0x06: Modem status register (R)
     * 0x07: Scratch register (R/W)
     *
     * Be careful though, because when you set the DLAB bit in the line control register,
     * the data register becomes the least significant byte of the divisor latch register.
     * The interrupt enable register becomes the most significant byte of the divisor latch register.
     **/

    // Disable interrupts.
    address[1] = 0x00;

    // Enable DLAB (to set baud rate divisor).
    address[3] = 0x80;

    //TODO: Lower the BAUD RATE

    // Sed divisor to 3 (low byte) and 0 (high byte) for 38400 baud rate.
    address[0] = 0x03;
    address[1] = 0x00;

    // Disable DLAB, 8 bits, no parity, one stop bit.
    address[3] = 0x03;

    // Enable FIFO, clear them, with 1-byte threshold.
    address[2] = 0x07;

    // IRQs enabled, RTS/DSR set.
    address[1] = 0x01;

    // IRQs enabled and OUT1 and OUT2 set.
    address[4] = 0x0F;

    DebugConsole::println("NS16550ADriver: initialized.");
  }

  void NS16550ADriver::shutdown() {
  }

  ErrorOr<String> NS16550ADriver::read() {
    String result;
    while(true) {
      auto error_or_char = read_char();
      if(error_or_char.has_error()) {
        break;
      }

      result += error_or_char.get_value();
    }

    return result;
  }

  void NS16550ADriver::write(String& message) {
    auto address = reinterpret_cast<u8*>(m_device->get_underlying_device()->as<UniversalDevice>()->get_address());
    for(auto i = 0; i < message.length(); i++) {
      address[0] = message[i];
    }
  }

  ErrorOr<char> NS16550ADriver::read_char() {
    auto address = reinterpret_cast<u8*>(m_device->get_underlying_device()->as<UniversalDevice>()->get_address());
    if((address[5] & 1) == 0) {
      return ErrorOr<char>::create_error(Error::create_from_string("No data available."));
    }

    return ErrorOr<char>::create(address[0]);
  }

  void NS16550ADriver::write(char c) {
    auto address = reinterpret_cast<u8*>(m_device->get_underlying_device()->as<UniversalDevice>()->get_address());
    address[0] = c;
  }

  void NS16550ADriver::handle_interrupt(u64 interrupt_id) {
    auto error_or_char = read_char();
    if(error_or_char.has_error()) {
      DebugConsole::println("NS16550ADriver: No data available.");
      return;
    } else {
      DebugConsole::print("NS16550ADriver: Received character: ");
      DebugConsole::print(error_or_char.get_value());
      DebugConsole::print('\n');
    }
  }

  ErrorOr<RefPtr<Kernel::DeviceDriver>> NS16550ADriverDescriptor::instantiate_driver(RefPtr<Device> device) {
    DebugConsole::println("NS16550ADriverDescriptor: Instantiating driver.");
    return ErrorOr<RefPtr<DeviceDriver>>::create(RefPtr<DeviceDriver>(new NS16550ADriver()));
  }

  bool NS16550ADriverDescriptor::is_compatible_with(RefPtr<Device> device) {
    DebugConsole::println("NS16550ADriverDescriptor: Checking compatibility with device.");
    if(device->get_device_type() != DeviceType::Console) {
      DebugConsole::println("NS16550ADriverDescriptor: Device is not a console device.");
      return false;
    }

    auto underlying_device = device->get_underlying_device();
    if(underlying_device->get_device_type() != UnderlyingDeviceType::Universal) {
      DebugConsole::println("NS16550ADriverDescriptor: Underlying device is not a universal device.");
      return false;
    }

    DebugConsole::print("Device type: ");
    DebugConsole::println(underlying_device->as<UniversalDevice>()->get_device_type());
    if(!underlying_device->as<UniversalDevice>()->get_device_type().equals_ignore_case("ns16550a")) {
      DebugConsole::println("NS16550ADriverDescriptor: Device is not an NS16550A device.");
      return false;
    }
    DebugConsole::println("NS16550ADriverDescriptor: Device is compatible.");
    return underlying_device->as<UniversalDevice>()->get_device_type().equals_ignore_case("ns16550a");
  }
}// namespace Kernel