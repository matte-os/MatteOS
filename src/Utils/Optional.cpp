#include <Utils/Optional.h>
#include <Utils/Types.h>

namespace Utils
{
    template <class E>
    Optional<E>::Optional()
    {
        this->has = false;
    }

    template <class E>
    Optional<E>::Optional(E value)
    {
        this->has = true;
        this->value = value;
    }

    template <class E>
    bool Optional<E>::has_some()
    {
        return has;
    }

    template <class E>
    E Optional<E>::get_value()
    {
        return value;
    }

    template class Optional<int>;
    template class Optional<char>;
    template class Optional<size_t>;
    template class Optional<u32>;
};