/**
 * @file Utility.h
 *
 * @author Matěj Bucek
 */

#pragma once

namespace Utils {
  template<typename T>
  void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
  }

  template<typename T>
  struct remove_reference {
    using type = T;
  };

  template<typename T>
  struct remove_reference<T&> {
    using type = T;
  };

  template<typename T>
  struct remove_reference<T&&> {
    using type = T;
  };

  template<typename T>
  struct is_lvalue_reference {
    static constexpr bool value = false;
  };

  template<typename T>
  struct is_lvalue_reference<T&> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct is_lvalue_reference<T&&> {
    static constexpr bool value = false;
  };

  template<typename T>
  constexpr T&& forward(typename remove_reference<T>::type&& arg) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(arg);
  }

  template<typename T>
  constexpr T&& forward(typename remove_reference<T>::type& arg) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(arg);
  }

  template<class T>
  constexpr typename remove_reference<T>::type&& move(T&& value) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(value);
  }

  template<typename T, typename U>
  struct is_same {
    const bool value = false;
  };

  template<typename T>
  struct is_same<T, T> {
    static const bool value = true;
  };

  template<typename T, typename U>
  concept same_as = is_same<T, U>::value;

  template<typename T>
  struct IsPointer {
    static const bool value = false;
  };

  template<typename T>
  struct IsPointer<T*> {
    static const bool value = true;
  };

  template<typename T>
  concept Pointer = IsPointer<T>::value;

}// namespace Utils