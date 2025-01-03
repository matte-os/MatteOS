//
// Created by matejbucek on 19.6.24.
//

#pragma once

#include <Utils/Basic.h>
#include <Utils/Strings/String.h>
#include <Utils/Strings/StringBuilder.h>

namespace Utils {
  using Utils::String;

  struct FormatterContext {
    bool escaped = false;
    bool open = false;
    String flags{};
    String result{};
    const char *format{};
  };

  // `Stringifiable` concept for non-pointers
  template <typename T>
  concept Stringifiable = requires(T t) {
    { to_string(t) };
  };

  // `StringifiablePointer` concept for pointers
  template <typename T>
  concept StringifiablePointer = Pointer<T> && requires(T t) {
    { to_string(t) };
  };

  // `StringifiableWithFlags` concept for types that use flags
  template <typename T>
  concept StringifiableWithFlags = requires(T t, String &flags) {
    { to_string(t, flags) };
  };

  void process(FormatterContext &context);

  template <typename T, typename... Args>
  void process(FormatterContext &context, const T &value, const Args &...args) {
    while (*context.format) {
      if (*context.format == '{' && !context.escaped) {
        context.open = true;
      } else if (*context.format == '}' && !context.escaped && context.open) {
        context.open = false;
        if constexpr (StringifiableWithFlags<T>) {
          context.result += to_string(value, context.flags);
        } else if constexpr (Stringifiable<T>) {
          context.result += to_string(value);
        } else if constexpr (StringifiablePointer<T>) {
          context.result += to_string(value);
        } else {
          static_assert(false, "No formatter found for the type T");
        }
        context.format++;
        process(context, args...);
        return;
      } else if (*context.format == '\\') {
        if (context.escaped) {
          context.escaped = false;
          context.result += *context.format;
        } else {
          context.escaped = true;
        }
      } else {
        context.escaped = false;
        if (context.open) {
          context.flags += *context.format;
        } else {
          context.result += *context.format;
        }
      }

      context.format++;
    }
  }

  template <typename... Args>
  String format(const char *format, const Args &...args) {
    FormatterContext context{};
    context.format = format;
    process(context, args...);
    return context.result;
  }
}// namespace Utils
