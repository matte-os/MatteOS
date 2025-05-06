/**
 * @file Traits.h
 *
 * @author Matěj Bucek
 *
 * This file is inspired by SerenityOS
 */

#pragma once

#include <Utils/Types.h>

namespace Utils {
  template<typename T>
  void swap(T& a, T& b) noexcept {
    T tmp = a;
    a = b;
    b = tmp;
  }

  template<typename T>
  struct RemoveReference {
    using type = T;
  };

  template<typename T>
  struct RemoveReference<T&> {
    using type = T;
  };

  template<typename T>
  struct RemoveReference<T&&> {
    using type = T;
  };

  template<typename T>
  struct RemovePointer {
    using type = T;
  };

  template<typename T>
  struct RemovePointer<T*> {
    using type = T;
  };

  template<typename T>
  struct IsLValueReference {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsLValueReference<T&> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct IsLValueReference<T&&> {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsRValueReference {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsRValueReference<T&> {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsRValueReference<T&&> {
    static constexpr bool value = true;
  };

  template<typename T>
  constexpr T&& forward(typename RemoveReference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
  }

  template<typename T>
  constexpr T&& forward(typename RemoveReference<T>::type&& arg) noexcept {
    return static_cast<T&&>(arg);
  }

  template<class T>
  constexpr typename RemoveReference<T>::type&& move(T&& value) noexcept {
    return static_cast<typename RemoveReference<T>::type&&>(value);
  }

  template<typename T, typename U>
  struct IsSame {
    const bool value = false;
  };

  template<typename T>
  struct IsSame<T, T> {
    static constexpr bool value = true;
  };

  template<typename T, typename U>
  concept SameAs = IsSame<T, U>::value;

  template<typename T>
  struct IsArray {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsArray<T[]> {
    static constexpr bool value = true;
  };

  template<typename T, size_t N>
  struct IsArray<T[N]> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct IsPointer {
    static constexpr bool value = false;
  };

  template<typename T>
  struct IsPointer<T*> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct IsPointer<const T*> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct IsPointer<volatile T*> {
    static constexpr bool value = true;
  };

  template<typename T>
  struct IsPointer<const volatile T*> {
    static constexpr bool value = true;
  };

  template<typename T>
  concept Pointer = IsPointer<T>::value || IsArray<T>::value;

  struct TrueType {
    static constexpr bool value = true;
  };

  struct FalseType {
    static constexpr bool value = false;
  };

  template<typename Base, typename Derived>
  struct IsConvertibleHelper {
    static FalseType test(...);

    template<typename T>
    static auto test(T* t) -> decltype(static_cast<Base*>(t), TrueType());

    using Type = decltype(test(static_cast<Derived*>(nullptr)));
    static constexpr bool value = Type::value;
  };

  template<typename Base, typename Derived>
  struct IsBaseOf {
    static constexpr bool value = IsConvertibleHelper<const volatile Base, Derived>::value;
  };

  template<bool Condition, class T = void>
  struct EnableIf {};

  template<class T>
  struct EnableIf<true, T> {
    typedef T type;
  };

  template<typename T>
  struct AddRValueReference {
    using type = T&&;
  };

  template<typename T>
  struct AddRValueReference<T&> {
    using type = T&;
  };

  template<typename T>
  struct AddRValueReference<T&&> {
    using type = T&&;
  };

  template<>
  struct AddRValueReference<void> {
    using type = void;
  };

  template<>
  struct AddRValueReference<const void> {
    using type = const void;
  };

  template<>
  struct AddRValueReference<volatile void> {
    using type = volatile void;
  };

  template<>
  struct AddRValueReference<const volatile void> {
    using type = const volatile void;
  };

  template<typename T>
  typename AddRValueReference<T>::type declval() noexcept;

  template<typename T>
  struct IsEnum {
    static const bool value = __is_enum(T);
  };

  template<typename T>
  constexpr bool IsEnumValue = IsEnum<T>::value;

  template<typename T, bool value = IsEnum<T>::value>
  struct UnderlyingType;

  template<typename T>
  struct UnderlyingType<T, true> {
    using type = __underlying_type(T);
  };

  template<typename T>
  using UnderlyingTypeValue = typename UnderlyingType<T>::type;

  template<typename T>
  typename UnderlyingType<T>::type as_underlying(T value) {
    return static_cast<typename UnderlyingType<T>::type>(value);
  }
}// namespace Utils