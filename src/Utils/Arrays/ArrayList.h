//
// Created by matejbucek on 2.9.22.
//

#pragma once
#include <Utils/Types.hh>
#include <Kernel/Memory/MemoryRegion.h>
#include <Utils/Pointers/RefCounted.h>

namespace Utils {
    template <typename T>
    class ArrayList : public Pointers::RefCounted<ArrayList<T>> {
    private:
        static const size_t DEFAULT_SIZE = 8;
        T* m_array;
        size_t m_size;
        size_t m_ptr;
    public:
        ArrayList();
        ~ArrayList();
        void add(T);
        T add_and_return(T element) { add(element); return element; }
        T get(size_t);
        void remove(size_t);
        //T operator[](size_t i) { return get(i); };
        size_t size();
        T* to_array() { return m_array; }
    private:
        void grow();
    };
}
