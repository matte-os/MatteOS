#pragma once

namespace Utils{
    template<class E>
    class Optional{
        bool has;
        E value;
        public:
            Optional();
            Optional(E value);
            bool has_some();
            E get_value();
    };
};