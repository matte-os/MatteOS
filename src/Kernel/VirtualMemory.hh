#pragma once
#include <Utils/Types.hh>
#include <Kernel/PagingTable.hh>

namespace Kernel{
    enum AllocListFlags : size_t{
        AL_TAKEN = 1 << 63
    };

    class AllocList{
        size_t flagsSize;
        public:
            bool isTaken();
            bool isFree();
            void setTaken();
            void setFree();
            void setSize(size_t);
            size_t getSize();

    };

    class VirtualMemory{
    private:
        static AllocList* KMEM_HEAD;
        static size_t KMEM_ALLOC;
        static PagingTable* KMEM_PAGE_TABLE;
    public:
        static void init();
        static PagingTable* getPagingTable();
        static AllocList* getHead();
        static size_t getNumAllocations();
        //static void kzmalloc();
        //static void kmalloc();
        //static void kfree();
    };
};