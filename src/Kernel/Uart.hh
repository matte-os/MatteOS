#pragma once
#include <Utils/Types.hh>
#include <Utils/Optional.hh>

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