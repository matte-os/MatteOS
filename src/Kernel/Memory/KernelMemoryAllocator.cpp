/**
 * @file KernelMemoryAllocator.cpp
 * @author Matěj Bucek
 *
 * The allocator is inspired by the memory allocator implemented in the
 * repository osblog written by Stephen Marz
 */
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static KernelMemoryAllocator* s_kernel_memory_allocator = nullptr;

  void KernelMemoryAllocator::init(uintptr_t* page) {
    s_kernel_memory_allocator = new(page) KernelMemoryAllocator;
  }

  KernelMemoryAllocator& KernelMemoryAllocator::the() {
    runtime_assert(s_kernel_memory_allocator, "KernelMemoryAllocator not initialized");
    return *s_kernel_memory_allocator;
  }

  KernelMemoryAllocator::KernelMemoryAllocator() {
    m_head = (AllocHeader*) MemoryManager::the().zalloc(ALLOCATION_SIZE);
    m_total_size = ALLOCATION_SIZE * MemoryManager::PAGE_SIZE;
    m_taken = 0;
    m_head->set_free();
    m_head->set_size(m_total_size);
  }

  KernelMemoryAllocator::~KernelMemoryAllocator() {
    MemoryManager::the().dealloc((uintptr_t*) m_head);
  }

  uintptr_t* KernelMemoryAllocator::kmalloc(size_t sz) {
    if(sz == 0) {
      DebugConsole::println("Warning: kmalloc called with size 0");
      return nullptr;
    }
    auto size = MemoryManager::align_value(sz, 3) + sizeof(AllocHeader);
    auto* ptr = m_head;
    auto* tail = (AllocHeader*) ((u8*) m_head) + m_total_size;
    while(ptr < tail) {
      if(ptr->is_free() && ptr->get_size() >= size) {
        auto chunkSize = ptr->get_size();
        auto remaining = chunkSize - size;
        ptr->set_taken();
        if(remaining > sizeof(AllocHeader)) {
          auto* next = (AllocHeader*) (((u8*) ptr) + size);
          next->set_free();
          next->set_size(remaining);
          ptr->set_size(size);
        } else {
          ptr->set_size(chunkSize);
        }
        return (uintptr_t*) ptr + 1;
      } else {
        ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
      }
    }

    return nullptr;
  }

  uintptr_t* KernelMemoryAllocator::kzmalloc(size_t sz) {
    auto size = MemoryManager::align_value(sz, 3);
    auto mem = kmalloc(size);

    for(size_t i = 0; i < size; i++) {
      *(((u8*) mem) + 1) = 0;
    }

    return mem;
  }

  void KernelMemoryAllocator::kfree(uintptr_t* ptr) {
    if(ptr != nullptr) {
      auto* alloc = ((AllocHeader*) ptr) - 1;
      alloc->set_free();
      coalesce();
    }
  }

  void KernelMemoryAllocator::debug() {
    auto* ptr = m_head;
    auto* tail = (AllocHeader*) (((u8*) m_head) + m_total_size);
    AllocHeader* prev = nullptr;

    u64 taken = 0;
    u64 free = 0;
    while(ptr < tail) {
      if(ptr->is_taken()) {
        taken += ptr->get_size();
      } else {
        if(ptr->get_size() > 1000000) {
          DebugConsole::print("Abnormal free block size: ");
          DebugConsole::print_ln_number(ptr->get_size(), 10);
        }
        free += ptr->get_size();
      }
      prev = ptr;
      ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
    }

    DebugConsole::print("Taken: ");
    DebugConsole::print_ln_number(taken, 10);
    DebugConsole::print("Free Counted: ");
    DebugConsole::print_ln_number(free, 10);
    DebugConsole::print("Free: ");
    DebugConsole::print_ln_number(m_total_size - taken, 10);
  }

  KernelMemoryAllocator::Statistics KernelMemoryAllocator::get_statistics() {
    auto* ptr = m_head;
    auto* tail = (AllocHeader*) (((u8*) m_head) + m_total_size);
    AllocHeader* prev = nullptr;

    u64 taken = 0;
    u64 free = 0;
    while(ptr < tail) {
      if(ptr->is_taken()) {
        taken += ptr->get_size();
      } else {
        free += ptr->get_size();
      }
      prev = ptr;
      ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
    }

    return {m_total_size, free, taken};
  }

  void KernelMemoryAllocator::coalesce() {
    auto* ptr = m_head;
    auto* tail = (AllocHeader*) (((u8*) m_head) + m_total_size);
    while(ptr < tail) {
      auto* next = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
      if(next >= tail || ptr->get_size() == 0) {
        break;
      } else if(ptr->is_free() && next->is_free()) {
        ptr->set_size(ptr->get_size() + next->get_size());
        if(ptr->get_size() > 1000000) {
          DebugConsole::print("Abnormal coalesced block size: ");
          DebugConsole::print_ln_number(ptr->get_size(), 10);
        }
      }
      ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
    }
  }
}// namespace Kernel

namespace std {// NOLINT(cert-dcl58-cpp) These declarations must be in ::std and we are not using <new>
  struct nothrow_t {
    explicit nothrow_t() = default;
  };

  void terminate() {}

  extern const nothrow_t nothrow;

  enum class align_val_t : size_t {};
};// namespace std

uintptr_t* kmalloc(size_t size) {
  return Kernel::KernelMemoryAllocator::the().kmalloc(size);
}

void kfree(uintptr_t* ptr) {
  Kernel::KernelMemoryAllocator::the().kfree(ptr);
}

void* operator new(size_t size) {
  return Kernel::KernelMemoryAllocator::the().kmalloc(size);
}

void* operator new[](size_t size) {
  return Kernel::KernelMemoryAllocator::the().kmalloc(size);
}

void operator delete(void* ptr) {
  if(!ptr) return;
  return Kernel::KernelMemoryAllocator::the().kfree((uintptr_t*) ptr);
}

void operator delete[](void* ptr) {
  if(!ptr) return;
  return Kernel::KernelMemoryAllocator::the().kfree((uintptr_t*) ptr);
}