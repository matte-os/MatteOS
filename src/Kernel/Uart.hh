#pragma once
#include <Utils/Types.hh>
#include <Utils/Option.hh>

namespace Kernel{
    class Uart {
        size_t baseAddress;
    public:
        Uart(size_t baseAddress);
        void init();
        void put(const char c);
        Utils::Option<char> get();
    };
};