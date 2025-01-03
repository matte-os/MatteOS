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

  void process(FormatterContext& context) { context.result += context.format; }
}// namespace Utils