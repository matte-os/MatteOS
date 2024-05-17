//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Utils/Types.h>

namespace Kernel::Memory {
    struct AllocHeader {
        u64 size:   63;
        u64 taken:  1;
    public:
        bool is_taken() { return taken == 1; }
        bool is_free() { return !is_taken(); }
        void set_taken() { taken = 1; }
        void set_free() { taken = 0; }
        u64 get_size() { return size; }
        void set_size(u64 size) { this->size = size; }
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
}

uintptr_t* kmalloc(size_t);
void kfree(uintptr_t*);

inline void* operator new(size_t, void* p) { return p; }
inline void* operator new[](size_t, void* p) { return p; }
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);