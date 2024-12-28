#pragma once

#include <Utils/Callable.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

namespace Utils {

  template<typename T>
  class LinkedListElement;

  template<typename T>
  class LinkedList : public RefCounted<LinkedList<T>> {
  private:
    LinkedListElement<T>* m_head;
    LinkedListElement<T>* m_tail;
    size_t m_size;

  public:
    LinkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

    LinkedList(const LinkedList<T>& other) = delete;

    LinkedList(LinkedList<T>&& other) {
      m_head = other.m_head;
      m_tail = other.m_tail;
      m_size = other.m_size;
      other.m_head = nullptr;
      other.m_tail = nullptr;
      other.m_size = 0;
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) = delete;

    LinkedList<T>& operator=(LinkedList<T>&& other) {
      if(this != &other) {
        m_head = other.m_head;
        m_tail = other.m_tail;
        m_size = other.m_size;
        other.m_head = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
      }
      return *this;
    }

    ~LinkedList() {
      auto head = m_head;
      while(head) {
        auto next = head->m_next;
        delete head;
        head = next;
      }
    }

    void append(T data) {
      auto new_element = new LinkedListElement<T>(data);
      if(!m_head) {
        m_head = new_element;
        m_tail = new_element;
      } else {
        m_tail->m_next = new_element;
        new_element->m_prev = m_tail;
        m_tail = new_element;
      }

      m_size++;
    }

    void prepend(T data) {
      auto new_element = new LinkedListElement<T>(data);
      if(!m_head) {
        m_head = new_element;
        m_tail = new_element;
      } else {
        m_head->m_prev = new_element;
        new_element->m_next = m_head;
        m_head = new_element;
      }

      m_size++;
    }

    void remove(size_t index) {
      if(index >= m_size) {
        return;
      }

      auto head = m_head;
      for(int i = 0; i < index; i++) {
        if(!head) {
          return;
        }

        if(i == index) {
          if(head->m_prev) {
            head->m_prev->m_next = head->m_next;
          }
          if(head->m_next) {
            head->m_next->m_prev = head->m_prev;
          }
          delete head;
          m_size--;
          return;
        }

        head = head->m_next;
      }
    }

    ErrorOr<T> get(size_t index) {
      if(index >= m_size) {
        return Error::create_from_string("Index out of bounds!");
      }

      auto head = m_head;
      for(int i = 0; i < index; i++) {
        if(!head) {
          return Error::create_from_string("Index out of bounds!");
        }

        if(i == index) {
          return {head->m_data};
        }

        head = head->m_next;
      }
    }

    ErrorOr<T> find_first_match(Callable<bool, const T&> predicate) {
      auto head = m_head;
      while(head) {
        if(predicate(head->m_data)) {
          return ErrorOr<T>::create(head->m_data);
        }
        head = head->m_next;
      }

      return Error::create_from_string("No match found!");
    }

    [[nodiscard]] size_t size() const {
      return m_size;
    }
  };

  template<typename T>
  class LinkedListElement {
  private:
    friend class LinkedList<T>;
    T m_data;
    LinkedListElement<T>* m_next;
    LinkedListElement<T>* m_prev;

  public:
    LinkedListElement(T data) : m_data(data), m_next(nullptr), m_prev(nullptr) {}

    LinkedListElement(const LinkedListElement<T>& other) = delete;
    LinkedListElement(LinkedListElement<T>&& other) = delete;
    LinkedListElement<T>& operator=(const LinkedListElement<T>& other) = delete;
    LinkedListElement<T>& operator=(LinkedListElement<T>&& other) = delete;
  };

}// namespace Utils
