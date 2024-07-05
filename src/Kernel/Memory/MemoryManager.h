//
// Created by matejbucek on 31.8.22.
//

#pragma once

#include <Kernel/Memory/Page.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Utils/Types.h>

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
        size_t m_heap_start;
        size_t m_heap_size;
        Page* m_first_page;
        size_t m_number_of_pages;
        size_t m_alloc_start;
    private:
        MemoryManager();

        static size_t align_value(size_t value, size_t order);
    public:
        static MemoryManager& the();
        static MemoryManager* ptr();
        static void init(size_t, size_t);
        uintptr_t* alloc(size_t);
        uintptr_t* zalloc(size_t);
        void dealloc(uintptr_t*);
        void identity_map_range(PageTable &root, size_t start, size_t end, u64 bits);
        void map_range(PageTable &root, size_t virtual_start, size_t virtual_end, size_t physical_start, u64 bits);
        void map(PageTable&, VirtualAddress, PhysicalAddress, size_t, int);
        void debug_output();
        PageTable* get_current_root_page_table();

        ~MemoryManager();

    public:
        static const size_t PAGE_ORDER = 12;
        static const size_t PAGE_SIZE = 1 << 12;
    };
}
