#pragma once

#include <Kernel/Memory/PhysicalAddress.h>
#include <Utils/Pointers/RefCounted.h>

namespace Kernel {

  class PhysicalPage : public RefCounted<PhysicalPage> {
    PhysicalAddress m_physical_address;

  public:
    ~PhysicalPage() override = default;
  };

}// namespace Kernel
