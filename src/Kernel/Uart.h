#pragma once
#include <Utils/Optional.h>
#include <Utils/Types.h>

namespace Kernel{
    class Uart {
        size_t baseAddress;
    public:
        Uart(size_t baseAddress);
        void init();
        void put(const char c);
        Utils::Optional<char> get();
    };
};