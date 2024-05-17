//
// Created by matejbucek on 24.4.24.
//

#pragma once

/*
#include <Utils/Types.h>
#include <typeinfo>

namespace Utils {
  template<typename T, typename U>
  struct is_same {
    const bool value = false;
  };

  template<typename T>
  struct is_same<T, T> {
    const bool value = true;
  };

  template<typename T>
  T&& move(T& value) {
    return static_cast<T&&>(value);
  }

  template<typename T, typename... Types>
  struct VariantHelper {
    static const size_t size = sizeof(T) > VariantHelper<Types...>::size? sizeof(T) : VariantHelper<Types...>::size;
    inline static void destroy(size_t type_id, char* data) {
      if(type_id == typeid(T).hash_code()) {
        reinterpret_cast<T*>(data)->~T();
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
    size_t m_type_id;
    bool m_has_value;
    VariantData<m_helper::size> m_data;

    static inline size_t invalid_type() {
      return typeid(void).hash_code();
    }
  public:
    Variant() : m_type_id(invalid_type()), m_has_value(false) {}
    Variant(Variant<Types...>& other) {

    }
    Variant(Variant<Types...>&& other) {

    }
    template<typename T>
    T& as() {
      if(!m_has_value) {
        throw std::runtime_error("No value is set!");
      }

      if(typeid(T).hash_code() != m_type_id) {
        throw std::runtime_error("The variant contains a value of different type!");
      }

      return *reinterpret_cast<T*>(m_data.data);
    }

    template<typename T>
    void set(T&& value) {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      new(m_data.data) T(move(value));
      m_type_id = typeid(T).hash_code();
      m_has_value = true;
    }

    template<typename T>
    void set(T& value) {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      new(m_data.data) T(value);
      m_type_id = typeid(T).hash_code();
      m_has_value = true;
    }

    void clear() {
      if(m_has_value) {
        m_helper::destroy(m_type_id, m_data.data);
      }

      m_type_id = invalid_type();
      m_has_value = false;
    }
  };

  struct Test {
    Test() {
      std::cout << "Test constructed" << std::endl;
    }

    Test(Test&& other) {
      std::cout << "Move constructor used" << std::endl;
    }

    Test(Test& other) {
      std::cout << "Copy constructor used" << std::endl;
    }

    ~Test() {
      std::cout << "Test destructed" << std::endl;
    }
  };

  int main() {
    Variant<std::string, int, float, Test> variant;
    variant.set(std::string("Ahoj"));
    std::cout << variant.as<std::string>() << std::endl;
    variant.set<int>(5);
    std::cout << variant.as<int>() << std::endl;
    variant.set<float>(5.4);
    std::cout << variant.as<float>() << std::endl;
    variant.set<Test>({});
    auto& nvm = variant.as<Test>();
    Test t;
    variant.set<Test>(t);
  }

}// namespace Utils
*/