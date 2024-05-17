/**
 * @file Array.h
 * @brief Array class definition
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

namespace Utils {
    /**
     * @brief Array class
     *
     * @tparam T Type of elements in the array
     */
    template <typename T>
    class Array : public RefCounted<Array<T>> {
    private:
        T* m_array; //!< The underlying array
        size_t m_size; //!< The size of the array
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
        //T operator[](size_t i) { return get(i); };
        /**
         * @brief Gets the size of the array
         *
         * @return The size of the array
         */
        size_t size() { return m_size; };
    };
}
