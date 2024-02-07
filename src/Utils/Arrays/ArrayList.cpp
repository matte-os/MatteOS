//
// Created by matejbucek on 2.9.22.
//

#include <Utils/Arrays/ArrayList.h>
#include <Utils/Memory.hh>
#include <Kernel/Process/Process.h>

namespace Utils {

    template <typename T>
    ArrayList<T>::ArrayList() {
        m_array = new T[DEFAULT_SIZE];
        m_size = DEFAULT_SIZE;
        m_ptr = 0;
    }

    template <typename T>
    ArrayList<T>::~ArrayList() {
        DebugConsole::println("ArrayList destroyed!");
        delete[] m_array;
    }

    template <typename T>
    void ArrayList<T>::add(T value) {
        if(m_ptr >= m_size) grow();
        m_array[m_ptr++] = value;
    }

    template <typename T>
    T ArrayList<T>::get(size_t index) {
        if(m_size > index) {
            return m_array[index];
        }
        return {};
    }

    template <typename T>
    void ArrayList<T>::remove(size_t i) {

    }

    template <typename T>
    size_t ArrayList<T>::size() {
        return m_ptr;
    }

    template <typename T>
    void ArrayList<T>::grow() {
        auto newSize = m_size * DEFAULT_SIZE;
        auto* tmpArray = new T[newSize];
        memcpy((char*)tmpArray, (char*) m_array, m_size);
        delete[] m_array;
        m_array = tmpArray;
        m_size = newSize;
    }
    template class ArrayList<Kernel::Memory::MemoryRegion>;
    template class ArrayList<Kernel::Process::Process*>;
    template class ArrayList<Kernel::Process::Thread*>;

}