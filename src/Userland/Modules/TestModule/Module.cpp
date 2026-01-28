#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/API/Module/DriverAPI.h>
#include <Utils/Pointers/RefPtr.h>
#include <Kernel/API/Module/Module.h>

class TestDriver : public Kernel::DeviceDriver {

};

class TestDriverDescriptor : public Kernel::DeviceDriverDescriptor {
};

extern "C" {
  int init_module(Kernel::Module::DriverAPI* driver_api) {
    driver_api->test();
  }

  int cleanup_module() {

  }
}

using Kernel::ModuleHeader;
DEFINE_MODULE("TestMod", init_module, cleanup_module);
MODULE_AUTHOR("Matěj Bucek");
MODULE_LICENSE("MIT");
MODULE_VERSION("1.0.0");
MODULE_DESCRIPTION("A test module for the kernel.");