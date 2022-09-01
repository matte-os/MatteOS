//
// Created by matejbucek on 31.8.22.
//

#pragma once

#include <Utils/Types.hh>
#include <Kernel/Memory/Page.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PhysicalAddress.h>

namespace Kernel::Memory {
    enum class EntryBits : u64{
        NONE = 0,
        VALID = 1 << 0,
        READ = 1 << 1,
        WRITE = 1 << 2,
        EXECUTE = 1 << 3,
        USER = 1 << 4,
        GLOBAL = 1 << 5,
        ACCESS = 1 << 6,
        DIRTY = 1 << 7,

        READ_WRITE = 1 << 1 | 1 << 2,
        READ_EXECUTE = 1 << 1 | 1 << 3,
        READ_WRITE_EXECUTE = 1 << 1 | 1 << 2 | 1 << 3,

        USER_READ_WRITE = 1 << 1 | 1 << 2 | 1 << 4,
        USER_READ_EXECUTE = 1 << 1 | 1 << 3 | 1 << 4,
        USER_READ_WRITE_EXECUTE = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4
    };
    class MemoryManager {
        friend class KernelMemoryAllocator;
    public:
        size_t heapStart;
        size_t heapSize;
        Page* firstPage;
        size_t numberOfPages;
        size_t allocStart;
    private:
        MemoryManager();

        static size_t alignValue(size_t, size_t);
    public:
        static MemoryManager& the();
        static MemoryManager* ptr();
        static void init(size_t, size_t);
        uintptr_t* alloc(size_t);
        uintptr_t* zalloc(size_t);
        void dealloc(uintptr_t*);
        void mapRange(PageTable&, size_t, size_t, u64);
        void map(PageTable&, VirtualAddress, PhysicalAddress, size_t, int);
        void debugOutput();
        void debugPageWalk(PageTable*);

        ~MemoryManager();

    public:
        static const size_t PAGE_ORDER = 12;
        static const size_t PAGE_SIZE = 1 << 12;
    };
}
