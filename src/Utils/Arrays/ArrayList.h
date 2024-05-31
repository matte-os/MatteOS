/**
 * @file ArrayList.h
 * @brief This class is a simple implementation of a dynamic array.
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once
#include <Kernel/Memory/MemoryRegion.h>
#include <Utils/Memory.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>
#include <Utils/DebugConsole.h>
#include <Utils/kmalloc.h>

namespace Utils {
  /**
   * @brief This class is a simple implementation of a dynamic array.
   *
   * @tparam T Type of the elements in the array.
   */
  template<typename T>
  class ArrayList : public Pointers::RefCounted<ArrayList<T>> {
  private:
    static const size_t DEFAULT_SIZE = 8;//!< Default size of the array.
    T* m_array;                          //!< Pointer to the array.
    size_t m_size;                       //!< Size of the array.
    size_t m_ptr;                        //!< Pointer to the last element in the array.

  public:
    /**
     * @brief Constructs a new ArrayList object.
     */
    ArrayList() {
      m_array = kmalloc<T>(sizeof(T) * DEFAULT_SIZE);
      m_size = DEFAULT_SIZE;
      m_ptr = 0;
    }

    /**
     * @brief Destroys the ArrayList object.
     */
    ~ArrayList() {
      DebugConsole::println("ArrayList destroyed!");
      for(size_t i = 0; i < m_ptr; i++) {
        m_array[i].~T();
      }
      kfree(m_array);
    }

    /**
     * @brief Adds an element to the array.
     *
     * @param value Element to be added.
     */
    void add(T value) {
      if(m_ptr >= m_size) grow();
      m_array[m_ptr++] = value;
    }

    /**
     * @brief Adds an element to the array and returns it.
     *
     * @param element Element to be added.
     * @return T Added element.
     */
    T add_and_return(T element) {
      add(element);
      return element;
    }

    /**
     * @brief Gets an element from the array.
     *
     * @param index Index of the element.
     * @return T Element at the index.
     */
    T get(size_t index) {
      if(m_size > index) {
        return m_array[index];
      }
      return {};
    }

    /**
     * @brief Removes an element from the array.
     *
     * @param index Index of the element.
     */
     //TODO: Implement remove method
    void remove(size_t index) {
    }
    //T operator[](size_t i) { return get(i); };

    /**
     * @brief Returns the size of the array.
     *
     * @return size_t Size of the array.
     */
    size_t size() {
      return m_ptr;
    }

    /**
     * @brief Returns the underlying array.
     *
     * @return T* Pointer to the underlying array.
     */
    T* to_array() { return m_array; }

  private:
    /**
     * @brief Grows the array.
     */
    void grow() {
      auto newSize = m_size * DEFAULT_SIZE;
      auto* tmpArray = kmalloc<T>(newSize);
      memcpy((char*) tmpArray, (char*) m_array, m_size);
      kfree(m_array);
      m_array = tmpArray;
      m_size = newSize;
    }
  };
}// namespace Utils
