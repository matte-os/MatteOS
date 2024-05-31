#pragma once
#include <Utils/Optional.h>
#include <Utils/Types.h>

namespace Kernel{
    class Uart {
        size_t baseAddress;
    public:
        explicit Uart(size_t baseAddress);
        void init();
        void put(char c);
        Utils::Optional<char> get();
    };
};