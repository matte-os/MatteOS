//
// Created by matejbucek on 19.6.24.
//

#pragma once

#include <Utils/Strings/String.h>
#include <Utils/Strings/StringBuilder.h>

namespace Utils {
  using Utils::String;

  struct StandardFormatter {
  };

  template<typename T>
  struct Formatter : public StandardFormatter {
  public:
    void format(StringBuilder& buffer, T arg) {
      buffer += arg.to_string();
    }
  };

  template<>
  struct Formatter<String> : public StandardFormatter {
  public:
    void format(StringBuilder& buffer, const String& arg) {
      buffer += arg;
    }
  };

  template<>
  struct Formatter<const char*> : public StandardFormatter {
  public:
    void format(StringBuilder& buffer, const char* arg) {
      buffer += arg;
    }
  };

  template<>
  struct Formatter<int> : public StandardFormatter {
    void format(StringBuilder& buffer, int arg) {
      buffer += String::from_int(arg);
    }
  };

  template<typename... Args>
  String format(const String& format, Args... args) {

  }

  template<typename T, typename... Args>
  void format(StringBuilder& buffer, StringView& fmt, T arg, Args... args) {
    for(size_t i = 0; i < fmt.length(); i++) {
      if(fmt[i] == '{') {
        Formatter<T> formatter;
        formatter.format(buffer, arg);
        format(buffer, fmt.offset(i + 1), args...);
        return;
      }
      buffer += fmt[i];
    }
  }

}// namespace Utils
