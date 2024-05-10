//
// Created by matejbucek on 28.10.22.
//

#pragma once
#include <Utils/Types.hh>
#include <Utils/Pointers/RefCounted.h>

namespace Utils {
    template <typename T>
    class Array : public RefCounted<Array<T>> {
    private:
        T* m_array;
        size_t m_size;
    public:
        explicit Array(size_t size) : m_size(size) {
            m_array = new T[size];
        }

        explicit Array(size_t size, T* array) : m_size(size) {
            m_array = array;
        }

        ~Array() {
            delete[] m_array;
        }
        void set(size_t index, T element) {
            m_array[index] = element;
        }
        T& get(size_t index) {
            return m_array[index];
        }
        T* get_underlying_array() {
            return m_array;
        }
        //T operator[](size_t i) { return get(i); };
        size_t size() { return m_size; };
    };
}
