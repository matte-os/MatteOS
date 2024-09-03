#pragma once

#include <Utils/Pointers/RefCounted.h>

namespace Kernel {
  using Utils::RefCounted;

  class OpenFileDescriptor : public RefCounted<OpenFileDescriptor> {

  };

  class OpenFileDescriptorTable {
  };
}// namespace Kernel
