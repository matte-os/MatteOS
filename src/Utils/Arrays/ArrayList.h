//
// Created by matejbucek on 2.9.22.
//

#pragma once
#include <Utils/Types.hh>
#include <Kernel/Memory/MemoryRegion.h>

namespace Utils {
    template <typename T>
    class ArrayList {
    private:
        static const size_t DEFAULT_SIZE = 8;
        T* array;
        size_t size;
        size_t ptr;
    public:
        ArrayList();
        ~ArrayList();
        void add(T);
        T get(size_t);
        void remove(size_t);
        T operator[] (size_t i) { return get(i); };
        size_t getSize();
    private:
        void grow();
    };

}
