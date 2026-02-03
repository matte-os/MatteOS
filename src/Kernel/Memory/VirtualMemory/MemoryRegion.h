/**
 * @file MemoryRegion.cpp
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/VirtualMemory/VMObject.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::String;

  enum class MemoryRegionPermissions : u8 {
    Read = 1 << 0,
    Write = 1 << 1,
    Execute = 1 << 2
  };

  class MemoryRegion : public RefCounted<MemoryRegion> {
    String m_name;
    u64 m_virtual_address;
    u64 m_size;
    RefPtr<VMObject> m_vm_object;
    PageTable* m_page_table;
    MemoryRegionPermissions m_permissions;

  public:
    MemoryRegion();
    ~MemoryRegion() override = default;

    [[nodiscard]] u64 get_size() const { return m_size; }

    [[nodiscard]] u64 get_start_address() const { return m_virtual_address; }

    [[nodiscard]] u64 get_end_address() const { return m_virtual_address + m_size; }

    [[nodiscard]] MemoryRegionPermissions get_permissions() const { return m_permissions; }

    [[nodiscard]] String get_name() const { return m_name; }

    [[nodiscard]] RefPtr<VMObject> get_vm_object() const { return m_vm_object; }
  };
}// namespace Kernel
