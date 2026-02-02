#pragma once

#include <Utils/Function.h>
#include <Utils/Types.h>

namespace Kernel {
  class ModuleContext{};

  using ModuleInit = Utils::Function<int, ModuleContext*>;
  using ModuleCleanup = Utils::Function<int>;

  struct ModuleHeader {
    u32 magic;
    u32 abi_version;
    ModuleInit init;
    ModuleCleanup cleanup;
  };

#define MODULE_ABI_VERSION 1
#define MODULE_MAGIC 0x4D4F4431

#define MODULE_INFO(key, value)       \
  static const char __modinfo_##key[] \
          __attribute__((section(".modinfo"), used)) = #key "=" value

#define DEFINE_MODULE(Name, Init, CleanUp)         \
  extern "C" {                                     \
  __attribute__((section(".module_header"), used)) \
  ModuleHeader __this_module_header = {            \
          MODULE_MAGIC,                            \
          MODULE_ABI_VERSION,                      \
          Init,                                    \
          CleanUp};                                \
  }

  // Standard keys
#define MODULE_AUTHOR(name) MODULE_INFO(author, name)
#define MODULE_LICENSE(lic) MODULE_INFO(license, lic)
#define MODULE_VERSION(ver) MODULE_INFO(version, ver)
#define MODULE_DESCRIPTION(desc) MODULE_INFO(description, desc)
}// namespace Kernel