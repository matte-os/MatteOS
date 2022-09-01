//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Utils/Types.hh>

namespace Kernel::Memory {
    struct AllocHeader {
        u64 size:   63;
        u64 taken:  1;
    public:
        bool isTaken() { return taken == 1; }
        bool isFree() { return !isTaken(); }
        void setTaken() { taken = 1; }
        void setFree() { taken = 0; }
        u64 getSize() { return size; }
        void setSize(u64 size) { this->size = size; }
    };
    class KernelMemoryAllocator {
    private:
        static const u64 ALLOCATION_SIZE = 64;
        AllocHeader* head;
        u64 totalSize;
        u64 taken;
    public:
        static void init(uintptr_t*);
        static KernelMemoryAllocator& the();
        uintptr_t* kmalloc(size_t);
        uintptr_t* kzmalloc(size_t);
        void kfree(uintptr_t*);
        void debug();
    private:
        KernelMemoryAllocator();
        ~KernelMemoryAllocator();
    };
}

inline void* operator new(size_t, void* p) { return p; }
inline void* operator new[](size_t, void* p) { return p; }
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);