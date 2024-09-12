#pragma once

#include <Utils/Arrays/Array.h>
#include <Utils/Hashing/HashFunctions.h>
#include <Utils/Maps/Map.h>
#include <Utils/Pointers/RefPtr.h>

namespace Utils {

  template<typename Key, typename Value, HashFunction<Key, u32> Function = HashFunctions::djb2, size_t DefaultSize = 20>
  class HashMap : public Map<Key, Value> {
  private:
    struct Entry : RefCounted<Entry> {
      Key key;
      Value value;
      RefPtr<Entry> next;
    };

    Array<RefPtr<Entry>> m_entries;

  public:
    HashMap() : m_entries(DefaultSize) {
      for(size_t i = 0; i < DefaultSize; i++) {
        m_entries.set(i, nullptr);
      }
    }

    HashMap(const HashMap<Key, Value, Function, DefaultSize>& other) : m_entries(other.m_entries) {
      static_assert(false, "Copy constructor not yet implemented!");
    }

    HashMap(HashMap<Key, Value, Function, DefaultSize>&& other) noexcept : m_entries(move(other.m_entries)) {
      other.m_entries = Array<RefPtr<Entry>>(DefaultSize);
    }

    ~HashMap() = default;

    HashMap<Key, Value, Function, DefaultSize>& operator=(const HashMap<Key, Value, Function, DefaultSize>& other) {
      static_assert(false, "Assignment operator not yet implemented!");
    }

    HashMap<Key, Value, Function, DefaultSize>& operator=(HashMap<Key, Value, Function, DefaultSize>&& other) noexcept {
      if(this != &other) {
        m_entries = move(other.m_entries);
        other.m_entries = Array<RefPtr<Entry>>(DefaultSize);
      }
      return *this;
    }

    ErrorOr<Value> get(const Key& key) override {
      u32 index = Function(key) % DefaultSize;
      RefPtr<Entry> entry = m_entries[index];

      while(entry != nullptr) {
        if(entry->key == key) {
          return ErrorOr<Value>::create(entry->value);
        }
        entry = entry->next;
      }

      return ErrorOr<Value>::create_error(Error::create_from_string("Key not found in map!"));
    }

    void set(const Key& key, const Value& value) override {
      u32 index = Function(key) % DefaultSize;
      RefPtr<Entry> entry = m_entries[index];

      if(entry == nullptr) {
        entry = RefPtr<Entry>(new Entry());
        entry->key = key;
        entry->value = value;
        m_entries.set(index, entry);
      } else {
        while(!entry->next) {
          entry = entry->next;
        }
        auto new_entry = RefPtr<Entry>(new Entry());
        new_entry->key = key;
        new_entry->value = value;
        entry->next = new_entry;
      }
    }

    void remove(const Key& key) override {
      u32 index = Function(key) % DefaultSize;
      RefPtr<Entry> entry = m_entries[index];
      RefPtr<Entry> previous;

      while(!entry) {
        if(entry->key == key) {
          if(previous == nullptr) {
            m_entries.set(index, entry->next);
          } else {
            previous->next = entry->next;
          }
          return;
        }
        previous = entry;
        entry = entry->next;
      }
    }

    bool has_key(const Key& key) {
      u32 index = Function(key) % DefaultSize;
      RefPtr<Entry> entry = m_entries[index];

      while(!entry) {
        if(entry->key == key) {
          return true;
        }

        entry = entry->next;
      }

      return false;
    }
  };

}// namespace Utils
