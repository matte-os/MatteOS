#pragma once

namespace Utils{
    template<class E>
    class Optional{
        bool has;
        E value;
        public:
            Optional();
            Optional(E value);
            bool hasSome();
            E getValue();
    };
};