#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Drivers/MMIODriver.h>
#include <Kernel/Firmware/DeviceTree.h>

namespace Kernel {

  ErrorOr<RefPtr<Driver>> MMIODriverDescriptor::instantiate_driver() {
    return ErrorOr<RefPtr<Driver>>::create(RefPtr<Driver>(new MMIODriver()));
  }

  void MMIODriver::init() {
    // Find the default serial console device
    auto error_or_serial = DeviceTree::the().find_node("/soc/serial");

    if(error_or_serial) {
      auto serial = error_or_serial.get_value();

      ArrayList<u64> interrupts;
      auto error_or_interrupts = serial->find_property("interrupts");
      if(error_or_interrupts) {
        auto interrupt = error_or_interrupts.get_value();
        interrupt->print();
        for(size_t i = 0; i < interrupt->number_of_u32_values(); i++) {
          interrupts.add(interrupt->get_value_as_u32(i));
        }
      }

      auto address = serial->get_address().to_uint(16);

      auto error_or_type = serial->find_property("compatible");
      String type = "unknown";
      if(error_or_type) {
        type = error_or_type.get_value()->get_value_as_string();
      }

      auto console = RefPtr<Device>(new ConsoleDevice(RefPtr<UnderlyingDevice>(new UniversalDevice(0, 0, "/soc/serial", type, address)), move(interrupts)));
      DeviceManager::the().add_device(console);
      console->init();
      DebugConsole::println("MMIODriver: Added serial console device.");
    }
    DebugConsole::println("MMIODriver: Initialized.");
  }

  void MMIODriver::shutdown() {
  }
}// namespace Kernel