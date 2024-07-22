#pragma once

#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Traits.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::Error;
  using Utils::ErrorOr;
  using Utils::Pointer;

  template<Pointer T>
  class Userspace {
  private:
    T* m_ptr;

  public:
    Userspace(T* ptr) : m_ptr(ptr) {}
    Userspace(uintptr_t ptr) : m_ptr(reinterpret_cast<T*>(ptr)) {}
    ~Userspace() = default;

    [[nodiscard]] VirtualAddress virtual_address() const { return reinterpret_cast<u64>(m_ptr); }
    [[nodiscard]] ErrorOr<T*> get(PageTable* root_page_table) const {
      auto error_or_value = root_page_table->translate(virtual_address());
      if(error_or_value.has_error()) {
        return ErrorOr<T*>::create_error(error_or_value.get_error());
      }

      return ErrorOr<T*>::create(reinterpret_cast<T*>(error_or_value.get_value().get_value()));
    }
  };
}// namespace Kernel
