//
// Created by matejbucek on 30.10.22.
//

#pragma once
#include <Utils/Pointers/RefCounted.h>

using Utils::Pointers::RefCounted;
namespace Utils {

    template<typename T>
    class QueueElement;

    template<typename T>
    class LinkedQueue : public RefCounted<LinkedQueue<T>>{
    private:
        QueueElement<T>* m_first_element;
        QueueElement<T>* m_last_element;
    public:
        LinkedQueue() : m_first_element(nullptr), m_last_element(nullptr) {}
        void clear() {
            m_first_element->delete_next();
            delete m_first_element;
            m_first_element = nullptr;
            m_last_element = nullptr;
        }
        void add(T element) {
            if(!m_last_element) {
                m_first_element = new QueueElement<T>(element, nullptr);
                m_last_element = m_first_element;
                return;
            }
            m_last_element->m_next = new QueueElement<T>(element, nullptr);
            m_last_element = m_last_element->m_next;
        }

        T add_and_return(T element) {
            add(element);
            return element;
        }

        T get(size_t i);

        size_t size() {
            size_t depth = 0;
            if(!m_first_element) return 0;
            m_first_element->get_depth(depth);
            return depth;
        }

        T first() {
            return m_first_element->m_element;
        }
        T pop() {
            auto temp = m_first_element->m_element;
            delete m_first_element;
            return temp;
        }
        T last() {
            return m_last_element->m_element;
        }
        void rotate_left() {
            if(size() <= 1) return;
            m_last_element->m_next = m_first_element;
            m_first_element = m_first_element->m_next;
            m_last_element = m_last_element->m_next;
            m_last_element->m_next = nullptr;
        }
        ~LinkedQueue() {
            clear();
        }
    };

    template<typename T>
    class QueueElement {
        friend class LinkedQueue<T>;
    private:
        T m_element;
        QueueElement<T>* m_next;
    public:
        QueueElement(T element, QueueElement<T>* next): m_element(element), m_next(next) {};
        ~QueueElement() = default;
        void get_depth(size_t& depth) {
            depth++;
            if(!m_next) return;
        }
        void delete_next() {
            if(!m_next) return;
            m_next->delete_next();
            delete m_next;
        }
    };

} // Kernel
