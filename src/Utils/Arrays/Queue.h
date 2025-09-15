/**
 * @file Queue.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Assertions.h>
#include <Utils/Memory.h>
#include <Utils/Types.h>

namespace Utils {
  template<typename T>
  class QueueElement;

  template<typename T>
  class LinkedQueue final {
    QueueElement<T>* m_head = nullptr;
    QueueElement<T>* m_tail = nullptr;

  public:
    LinkedQueue() : m_head(nullptr), m_tail(nullptr) {
    }

    LinkedQueue(const LinkedQueue& other) {
      for(auto current = other.m_head; current != nullptr; current = current->m_next) {
        append(current->m_element);
      }
    }

    LinkedQueue(LinkedQueue&& other) noexcept {
      m_head = other.m_head;
      m_tail = other.m_tail;
      other.m_head = nullptr;
      other.m_tail = nullptr;
    }

    LinkedQueue& operator=(const LinkedQueue& other) {
      if(this != &other) {
        clear();
        for(auto current = other.m_head; current != nullptr; current = current->m_next) {
          append(current->m_element);
        }
      }
      return *this;
    }

    LinkedQueue& operator=(LinkedQueue&& other) noexcept {
      if(this != &other) {
        clear();
        m_head = other.m_head;
        m_tail = other.m_tail;
        other.m_head = nullptr;
        other.m_tail = nullptr;
      }
      return *this;
    }

    ~LinkedQueue() {
      clear();
    }

    void clear() {
      auto current = m_head;
      while(current) {
        auto next = current->m_next;
        delete current;
        current = next;
      }
      m_head = nullptr;
      m_tail = nullptr;
    }

    void append(const T& element) {
      if(m_head == nullptr) {
        m_head = new QueueElement<T>(element);
        m_tail = m_head;
      } else {
        m_tail->m_next = new QueueElement<T>(element);
        m_tail = m_tail->m_next;
      }
    }

    void append(T&& element) {
      if(m_head == nullptr) {
        m_head = new QueueElement<T>(move(element));
        m_tail = m_head;
      } else {
        m_tail->m_next = new QueueElement<T>(move(element));
        m_tail = m_tail->m_next;
      }
    }

    T pop() {
      runtime_assert(!is_empty(), "Cannot pop from empty queue");

      T element = move(m_head->m_element);
      auto old_head = m_head;
      m_head = m_head->m_next;
      delete old_head;
      if(!m_head) m_tail = nullptr;

      return element;// return by value (NRVO/move optimizations will kick in)
    }

    const T& peek_first() {
      runtime_assert(!is_empty(), "Cannot peek from empty queue");
      return m_head->m_element;
    }

    const T& peek_last() {
      runtime_assert(!is_empty(), "Cannot peek from empty queue");
      return m_tail->m_element;
    }

    void rotate_left() {
      if(m_head == nullptr || m_head->m_next == nullptr) {
        return;// No need to rotate if the queue is empty or has only one element
      }

      auto old_head = m_head;
      m_head = m_head->m_next;
      m_tail->m_next = old_head;
      old_head->m_next = nullptr;
      m_tail = old_head;
    }

    bool is_empty() const {
      return m_head == nullptr;
    }
  };

  template<typename T>
  class QueueElement final {
    T m_element;
    QueueElement* m_next {nullptr};
    friend class LinkedQueue<T>;

  public:
    explicit QueueElement(const T& element) : m_element(element), m_next(nullptr) {}

    explicit QueueElement(T&& element) : m_element(move(element)), m_next(nullptr) {}
  };
}// namespace Utils
