#pragma once
#include <Utils/Types.hh>
#include <Kernel/Page.hh>

namespace Kernel{

    static size_t HEAP_START;
    static size_t HEAP_SIZE;
    static size_t ALLOC_START = 0;
    static const size_t PAGE_ORDER = 12;
    static const size_t PAGE_SIZE = 1 << 12;
    static size_t getHeapStart(){
        return (size_t)HEAP_START;
    }

    static size_t getHeapSize(){
        return (size_t)HEAP_SIZE;
    }


    static size_t alignValue(size_t value, size_t order){
        size_t o = (1 << order) - 1;
        return (value + o) & ~o;
    }

    class Paging{
    public: 
        Paging(){};
        static void init(size_t, size_t);
        static u8* alloc(size_t pages);
        static u8* zalloc(size_t pages);
        static void dealloc(u8* ptr);
    };
}