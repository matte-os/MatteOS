#include <Kernel/API/Module/Module.h>
#include <Kernel/Drivers/DeviceDriver.h>

class TestDriver : public Kernel::DeviceDriver {
};

class TestDriverDescriptor : public Kernel::DeviceDriverDescriptor {
};

extern "C" {
int init_module(Kernel::ModuleContext*) {
  return 0;
}

int cleanup_module() {
  return 0;
}
}

using Kernel::ModuleHeader;
DEFINE_MODULE("TestMod", init_module, cleanup_module);
MODULE_AUTHOR("Matěj Bucek");
MODULE_LICENSE("MIT");
MODULE_VERSION("1.0.0");
MODULE_DESCRIPTION("A test module for the kernel.");