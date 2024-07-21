#pragma once
#include <Utils/Strings/String.h>

namespace Kernel {
using Utils::StringView;
#ifdef __GNUC__
#define GET_CLASS_NAME (__PRETTY_FUNCTION__)
#else
#define GET_CLASS_NAME (__FUNCTION__)
#endif

template <typename... Args>
void log(StringView format, StringView prefix, Args... args) {

}

#define dbglog(...) log(GET_CLASS_NAME, __VA_ARGS__)
}// namespace Kernel
