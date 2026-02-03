#pragma once

#include <Kernel/Memory/VirtualMemory/PhysicalPage.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Pointers/RefCounted.h>

namespace Kernel {
  using Utils::HashMap;

  class VMObject : public RefCounted<VMObject> {
  public:
    virtual ~VMObject() = default;
    virtual PhysicalPage get_page(size_t offset) = 0;
    virtual void add_page(size_t offset, PhysicalPage physical_page_address) = 0;
  };

  class AnonymousVMObject : public VMObject {
    HashMap<size_t, PhysicalPage> m_pages;

  public:
    ~AnonymousVMObject() override = default;
    PhysicalPage get_page(size_t offset) override;
    void add_page(size_t offset, PhysicalPage physical_page_address) override;
  };
}// namespace Kernel
