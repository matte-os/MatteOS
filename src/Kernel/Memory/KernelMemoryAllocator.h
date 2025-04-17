/**
* @file KernelMemoryAllocator.cpp
 * @author Matěj Bucek
 *
 * The allocator is inspired by the memory allocator implemented in the
 * repository osblog written by Stephen Marz.
 */
#pragma once

#include <Utils/DebugConsole.h>
#include <Utils/Types.h>

using Utils::DebugConsole;

namespace Kernel {
  struct AllocHeader {
    u64 size  : 63;
    u64 taken : 1;

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

  static_assert(sizeof(AllocHeader) == 8, "AllocHeader size is not 8 bytes");

  class KernelMemoryAllocator {
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

    struct Statistics {
      u64 total_size;
      u64 free_size;
      u64 used_size;
    };

    Statistics get_statistics();
    bool is_in_freed_memory(uintptr_t* ptr);

  private:
    void coalesce();
    KernelMemoryAllocator();
    ~KernelMemoryAllocator();
  };
}// namespace Kernel

uintptr_t* kmalloc(size_t);
void kfree(uintptr_t*);

inline void* operator new(size_t, void* p) { return p; }

inline void* operator new[](size_t, void* p) { return p; }

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);