/**
 * @file ArrayList.h
 * @brief This class is a simple implementation of a dynamic array.
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once

#include <Kernel/Memory/MemoryRegion.h>
#include <Utils/Assertions.h>
#include <Utils/Callable.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Forwards.h>
#include <Utils/Function.h>
#include <Utils/Memory.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>
#include <Utils/kmalloc.h>

namespace Utils {
  /**
   * @brief This class is a simple implementation of a dynamic array.
   *
   * @tparam T Type of the elements in the array.
   */
  template<typename T>
  class ArrayList : public RefCounted<ArrayList<T>> {
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

    ArrayList(const ArrayList<T>& other) {
      m_array = kmalloc<T>(sizeof(T) * other.m_size);
      m_size = other.m_size;
      m_ptr = other.m_ptr;
      memcpy((char*) m_array, (char*) other.m_array, m_ptr * sizeof(T));
    }

    ArrayList(ArrayList<T>&& other) noexcept {
      m_array = other.m_array;
      m_size = other.m_size;
      m_ptr = other.m_ptr;
      other.m_array = nullptr;
      other.m_size = 0;
      other.m_ptr = 0;
    }

    /**
     * @brief Destroys the ArrayList object.
     */
    ~ArrayList() {
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
    size_t add(T value) {
      if(m_ptr >= m_size) grow();
      //Zero-initialize the memory
      memset((char*) &m_array[m_ptr], 0, sizeof(T));
      new(&m_array[m_ptr]) T(value);
      return m_ptr++;
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
    T& get(size_t index) {
      runtime_assert(index < m_size, "Array: Index out of bounds!");
      return m_array[index];
    }

    const T& get(size_t index) const {
      runtime_assert(index < m_size, "Array: Index out of bounds!");
      return m_array[index];
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
    size_t size() const {
      return m_ptr;
    }

    /**
     * @brief Returns the underlying array.
     *
     * @return T* Pointer to the underlying array.
     */
    T* to_array() { return m_array; }

    T& operator[](size_t i) { return get(i); }

    const T& operator[](size_t i) const { return get(i); }

    ArrayList<T>& operator=(const ArrayList<T>& other) {
      if(this == &other) return *this;
      m_array = kmalloc<T>(sizeof(T) * other.m_size);
      m_size = other.m_size;
      m_ptr = other.m_ptr;
      memcpy((char*) m_array, (char*) other.m_array, m_ptr * sizeof(T));
      return *this;
    }

    ArrayList<T>& operator=(ArrayList<T>&& other) noexcept {
      if(this == &other) return *this;
      m_array = other.m_array;
      m_size = other.m_size;
      m_ptr = other.m_ptr;
      other.m_array = nullptr;
      other.m_size = 0;
      other.m_ptr = 0;
      return *this;
    }

    ErrorOr<T> find_first_match(Callable<bool, const T&> predicate) {
      for(size_t i = 0; i < size(); i++) {
        if(predicate(get(i))) {
          return ErrorOr<T>::create(get(i));
        }
      }

      return ErrorOr<T>::create_error(Error::create_from_string("No match found!"));
    }

    ArrayList<T> find_all_matches(Callable<bool, const T&> predicate) {
      ArrayList<T> matches;
      for(size_t i = 0; i < size(); i++) {
        if(predicate(get(i))) {
          matches.add(get(i));
        }
      }

      return matches;
    }

    void for_each(Callable<void, const T&> func) const {
      for(size_t i = 0; i < size(); i++) {
        func(get(i));
      }
    }

    class Iterator {
    private:
      T* m_ptr;
    public:
      Iterator(T* ptr) : m_ptr(ptr) {}

      T& operator*() const { return *m_ptr; }
      T* operator->() { return m_ptr; }

      Iterator& operator++() {
        m_ptr++;
        return *this;
      }

      Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
      bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
    };

    Iterator begin() { return Iterator(m_array); }
    Iterator end() { return Iterator(m_array + m_ptr); }

    // Const versions
    Iterator begin() const { return Iterator(m_array); }
    Iterator end() const { return Iterator(m_array + m_ptr); }

  private:
    /**
     * @brief Grows the array.
     */
    void grow() {
      auto newSize = m_size * 2;
      auto* tmpArray = kmalloc<T>(newSize * sizeof(T));
      memcpy((char*) tmpArray, (char*) m_array, m_ptr * sizeof(T));
      kfree(m_array);
      m_array = tmpArray;
      m_size = newSize;
    }
  };
}// namespace Utils
