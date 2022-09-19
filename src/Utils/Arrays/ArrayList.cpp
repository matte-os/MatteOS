//
// Created by matejbucek on 2.9.22.
//

#include <Utils/Arrays/ArrayList.h>
#include <Utils/Memory.hh>

namespace Utils {

    template <typename T>
    ArrayList<T>::ArrayList() {
        array = new T[DEFAULT_SIZE];
        size = DEFAULT_SIZE;
        ptr = 0;
    }

    template <typename T>
    ArrayList<T>::~ArrayList() {
        delete[] array;
    }

    template <typename T>
    void ArrayList<T>::add(T value) {
        if(ptr >= size) grow();
        array[ptr++] = value;
    }

    template <typename T>
    T ArrayList<T>::get(size_t i) {
        if(size < i) {
            return array[i];
        }
    }

    template <typename T>
    void ArrayList<T>::remove(size_t i) {

    }

    template <typename T>
    size_t ArrayList<T>::getSize() {
        return ptr;
    }

    template <typename T>
    void ArrayList<T>::grow() {
        auto newSize = size * DEFAULT_SIZE;
        auto* tmpArray = new T[newSize];
        memcpy((char*)tmpArray, (char*) array, size);
        delete[] array;
        array = tmpArray;
        size = newSize;
    }
    template class ArrayList<Kernel::Memory::MemoryRegion>;
}