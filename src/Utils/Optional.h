#pragma once

namespace Utils{
    template<class E>
    class Optional{
        bool m_has_value;
        E m_value;
        public:
            Optional() : m_has_value(false) {}
            Optional(E value) : m_has_value(true), m_value(value) {}
            bool has_value() const { return m_has_value; }
            E get_value() const { return m_value; }
    };
};