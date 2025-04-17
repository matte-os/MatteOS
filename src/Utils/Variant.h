/**
 * @file Variant.h
 * @author Matěj Bucek (matejbucek)
 *
 * This code was inspired by Tibor Djurica Potpara (https://ojdip.net/2013/10/implementing-a-variant-type-in-cpp).
 */
#pragma once

#include <Utils/DebugConsole.h>
#include <Utils/Traits.h>
#include <Utils/Types.h>
#include <Utils/kmalloc.h>

namespace Utils {
  template<typename T, typename... Types>
  struct VariantHelper {
    static const size_t size = sizeof(T) > VariantHelper<Types...>::size ? sizeof(T) : VariantHelper<Types...>::size;
    inline static void destroy(size_t type_id, char* data) {
      if(type_id == TypeID<T>::get()) {
        using Type = typename remove_reference<T>::type;
        reinterpret_cast<Type*>(data)->~Type();
      }
    }
  };

  template<typename T>
  struct VariantHelper<T> {
    static const size_t size = sizeof(T);
  };

  template<size_t size>
  struct VariantData {
    char data[size];
  };

  template<typename... Types>
  class Variant {
  private:
    typedef VariantHelper<Types...> m_helper;
    size_t m_type_id{};
    bool m_has_value{};
    VariantData<m_helper::size> m_data;

    static inline size_t invalid_type() {
      return TypeID<void>::get();
    }

  public:
    Variant() : m_type_id(invalid_type()), m_has_value(false) {}
    Variant(Variant<Types...>& other) {
    }
    Variant(Variant<Types...>&& other) noexcept {
    }
    template<typename T>
    T& as() {
      if(!m_has_value) {
        DebugConsole::println("No value set!");
      }

      if(TypeID<T>::get() != m_type_id) {
        DebugConsole::println("Invalid type! Expected: ");
      }

      return *reinterpret_cast<T*>(m_data.data);
    }

    template<typename T>
    const T& as() const {
      if(!m_has_value) {
        DebugConsole::println("No value set!");
      }

      if(TypeID<T>::get() != m_type_id) {
        DebugConsole::println("Invalid type! Expected: ");
      }

      return *reinterpret_cast<const T*>(m_data.data);
    }

    template<typename T>
    void set(T&& value) {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      new(m_data.data) T(move(value));
      m_type_id = TypeID<T>::get();
      m_has_value = true;
    }

    template<typename T>
    void set(T& value) {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      new(m_data.data) T(value);
      m_type_id = TypeID<T>::get();
      m_has_value = true;
    }

    void clear() {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      m_type_id = invalid_type();
      m_has_value = false;
    }

    [[nodiscard]] bool has_value() const {
      return m_has_value;
    }

    template<typename T>
    [[nodiscard]] bool is() const {
      return m_type_id == TypeID<T>::get();
    }
  };

}// namespace Utils
