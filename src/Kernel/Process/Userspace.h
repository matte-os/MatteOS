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
    using PointerType = typename Utils::RemovePointer<T>::type;
    PointerType* m_ptr;

  public:
    Userspace(PointerType* ptr) : m_ptr(ptr) {}

    Userspace(uintptr_t ptr) : m_ptr(reinterpret_cast<PointerType*>(ptr)) {}

    ~Userspace() = default;

    [[nodiscard]] VirtualAddress virtual_address() const { return reinterpret_cast<u64>(m_ptr); }

    [[nodiscard]] ErrorOr<PointerType*> get(PageTable* root_page_table) const {
      auto error_or_value = root_page_table->translate(virtual_address());
      if(error_or_value.has_error()) {
        DebugConsole::print("Failed to translate address: ");
        DebugConsole::println(error_or_value.get_error().get_message().value());
        return ErrorOr<PointerType*>::create_error(error_or_value.get_error());
      }

      return ErrorOr<PointerType*>::create(reinterpret_cast<PointerType*>(error_or_value.get_value().get_value()));
    }
  };
}// namespace Kernel
