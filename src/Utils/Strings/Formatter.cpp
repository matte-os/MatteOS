#include <Utils/Strings/Formatter.h>

namespace Utils {
  String to_string(const String& value) { return value; }

  String to_string(const char* value) { return {value}; }

  String to_string(int value) {
    char buffer[64];
    itoa(buffer, value, 10);
    return {buffer};
  }

  String to_string(char value) {
    char buffer[2];
    buffer[0] = value;
    buffer[1] = '\0';
    return {buffer};
  }

  String to_string(bool value) {
    return value ? "true" : "false";
  }

  String to_string(size_t value) {
    char buffer[64];
    itoa(buffer, value, 10);
    return {buffer};
  }

  String to_string(size_t value, String& flags) {
    char buffer[64];

    size_t base = 10;
    if(flags == "16") {
      base = 16;
    } else if(flags == "2") {
      base = 2;
    }

    itoa(buffer, value, base);
    return {buffer};
  }

  void process(FormatterContext& context) { context.result += context.format; }
}// namespace Utils