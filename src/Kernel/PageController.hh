#pragma once

#include <Kernel/Page.hh>
#include <Kernel/PageTable.hh>
#include <Kernel/VirtualAddress.hh>
#include <Kernel/PhysicalAddress.hh>

namespace Kernel{
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

    class PageController{
        static size_t heapStart;
        static size_t heapSize;
        static Page* firstPage;
        static size_t numberOfPages;
        static size_t allocStart;
        static const size_t PAGE_ORDER = 12;
        public:
        static const size_t PAGE_SIZE = 1 << 12;
            PageController();
            ~PageController();
            static void init(size_t, size_t);
            static size_t* alloc(size_t);
            static size_t* zalloc(size_t);
            static void dealloc(size_t*);
            static void debugOutput();
            static void mapRange(PageTable&, size_t, size_t, u64);
            static void map(PageTable&, VirtualAddress, PhysicalAddress, size_t, int);
            static void debugPageWalk(PageTable*);
            static void allocAndMap();
        private:
            static size_t alignValue(size_t, size_t);
            static void debugPageWalkIdk(PageTable*, int);
    };
};