//
// Created by matejbucek on 30.10.22.
//

#pragma once
#include <Utils/Pointers/RefCounted.h>

using Utils::RefCounted;

namespace Utils {
    template<typename T>
    class QueueElement;

    template<typename T>
    class LinkedQueue final : public RefCounted<LinkedQueue<T> > {
        QueueElement<T>* m_first_element;
        QueueElement<T>* m_last_element;

    public:
        LinkedQueue() : m_first_element(nullptr), m_last_element(nullptr) {
        }

        void clear() {
            if (!m_first_element) return;
            m_first_element->delete_next();
            delete m_first_element;
            m_first_element = nullptr;
            m_last_element = nullptr;
        }

        void add(T element) {
            if (!m_last_element) {
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

        T get(size_t i) {
            QueueElement<T>* current = m_first_element;
            size_t index = 0;
            while (current) {
                if (index == i) return current->m_element;
                current = current->m_next;
                index++;
            }
            runtime_assert(true, "Index out of bounds");
        }

        size_t size() {
            size_t depth = 0;
            if (!m_first_element) return 0;
            m_first_element->get_depth(depth);
            return depth;
        }

        T first() {
            return m_first_element->m_element;
        }

        T pop() {
            runtime_assert(m_first_element, "Queue is empty");
            auto temp = m_first_element->m_element;
            auto old_first = m_first_element;
            m_first_element = m_first_element->m_next;
            delete old_first;
            if (!m_first_element) m_last_element = nullptr;
            return temp;
        }

        T last() {
            return m_last_element->m_element;
        }

        void rotate_left() {
            if (size() <= 1) return;
            m_last_element->m_next = m_first_element;
            m_first_element = m_first_element->m_next;
            m_last_element = m_last_element->m_next;
            m_last_element->m_next = nullptr;
        }

        ~LinkedQueue() override {
            clear();
        }

        bool is_empty() const {
            return m_first_element == nullptr;
        }
    };

    template<typename T>
    class QueueElement {
        friend class LinkedQueue<T>;
        T m_element;
        QueueElement<T>* m_next;

    public:
        QueueElement(T element, QueueElement<T>* next): m_element(element), m_next(next) {
        };

        ~QueueElement() {
            delete_next();
        }

        void get_depth(size_t& depth) const {
            depth++;
            if (m_next) {
                m_next->get_depth(depth);
            }
        }

        void delete_next() {
            if (!m_next) return;
            m_next->delete_next();
            delete m_next;
        }
    };
} // Kernel
