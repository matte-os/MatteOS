#pragma once

namespace Utils{
    template<class E>
    class Option{
        bool has;
        E value;
        public:
            Option();
            Option(E value);
            bool hasSome();
            E getValue();
    };
};