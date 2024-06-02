//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Utils/DebugConsole.h>
#include <Utils/Types.h>

using Utils::DebugConsole;

namespace Kernel::Memory {
  struct AllocHeader {
    u64 size  : 63;
    u64 taken : 1;

  public:
    [[nodiscard]] bool is_taken() const { return taken == 1; }
    [[nodiscard]] bool is_free() const { return !is_taken(); }
    void set_taken() { taken = 1; }
    void set_free() { taken = 0; }
    [[nodiscard]] u64 get_size() const { return size; }
    void set_size(u64 size) {
      if(size == 0) {
        DebugConsole::println("Warning: AllocHeader::set_size called with size 0");
      }

      if(size > 1000000) {
        DebugConsole::println("Warning: AllocHeader::set_size called with size > 1000000");
      }
      this->size = size;
    }
  };
  class KernelMemoryAllocator {
  private:
    static const u64 ALLOCATION_SIZE = 64;
    AllocHeader* m_head;
    u64 m_total_size;
    u64 m_taken;

  public:
    static void init(uintptr_t*);
    static KernelMemoryAllocator& the();
    uintptr_t* kmalloc(size_t);
    uintptr_t* kzmalloc(size_t);
    void kfree(uintptr_t*);
    void debug();

  private:
    void coalesce();
    KernelMemoryAllocator();
    ~KernelMemoryAllocator();
  };
}// namespace Kernel::Memory

uintptr_t* kmalloc(size_t);
void kfree(uintptr_t*);

inline void* operator new(size_t, void* p) { return p; }
inline void* operator new[](size_t, void* p) { return p; }
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);