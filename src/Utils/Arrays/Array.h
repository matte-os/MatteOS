/**
 * @file Array.h
 * @brief Array class definition
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once
#include <Utils/Assertions.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

namespace Utils {
  /**
     * @brief Array class
     *
     * @tparam T Type of elements in the array
     */
  template<typename T>
  class Array : public RefCounted<Array<T>> {
  private:
    T* m_array;   //!< The underlying array
    size_t m_size;//!< The size of the array
  public:
    /**
         * @brief Allocates sufficient memory for an array of size elements of type T
         *
         * @param size Size of the array
         */
    explicit Array(size_t size) : m_size(size) {
      m_array = new T[size];
    }

    /**
         * @brief Initializes the array with the given array
         *
         * @param size Size of the array
         * @param array The array to initialize the new array with
         */
    explicit Array(size_t size, T* array) : m_size(size) {
      m_array = array;
    }

    Array(const Array<T>& other) : m_size(other.m_size) {
      m_array = new T[m_size];
      for(size_t i = 0; i < m_size; i++) {
        m_array[i] = other.m_array[i];
      }
    }

    Array(Array<T>&& other) noexcept : m_size(other.m_size), m_array(other.m_array) {
      other.m_array = nullptr;
      other.m_size = 0;
    }

    Array<T>& operator=(const Array<T>& other) {
      if(this != &other) {
        m_size = other.m_size;
        delete[] m_array;
        m_array = new T[m_size];
        for(size_t i = 0; i < m_size; i++) {
          m_array[i] = other.m_array[i];
        }
      }
      return *this;
    }

    Array<T>& operator=(Array<T>&& other) noexcept {
      if(this != &other) {
        m_size = other.m_size;
        delete[] m_array;
        m_array = other.m_array;
        other.m_array = nullptr;
        other.m_size = 0;
      }
      return *this;
    }

    void fill(T value) {
      for(size_t i = 0; i < m_size; i++) {
        m_array[i] = value;
      }
    }

    /**
         * @brief Destructor
         */
    ~Array() {
      delete[] m_array;
    }

    /**
         * @brief Sets the element at the given index
         *
         * @param index Index of the element
         * @param element Element to set
         */
    void set(size_t index, T element) {
      m_array[index] = element;
    }

    /**
         * @brief Gets the element at the given index
         *
         * @param index Index of the element
         * @return Element at the given index
         */
    T& get(size_t index) {
      runtime_assert(index < m_size, "Array: Index out of bounds!");
      return m_array[index];
    }

    T& get(size_t index) const {
      runtime_assert(index < m_size, "Array: Index out of bounds!");
      return m_array[index];
    }

    /**
         * @brief Gets the underlying array
         *
         * @return The underlying array
         */
    T* get_underlying_array() {
      return m_array;
    }

    T& operator[](size_t i) { return get(i); };

    T& operator[](size_t i) const { return get(i); };

    /**
         * @brief Gets the size of the array
         *
         * @return The size of the array
         */
    size_t size() { return m_size; };
  };
}// namespace Utils
