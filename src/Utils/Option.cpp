#include <Utils/Option.hh>
#include <Utils/Types.hh>

namespace Utils{
   template<class E>
    Option<E>::Option(){
        this->has = false;
    }

    template<class E>
    Option<E>::Option(E value){
        this->has = true;
        this->value = value;
    }

    template<class E>
    bool Option<E>::hasSome(){
        return has;
    }

    template<class E>
    E Option<E>::getValue(){
        return value;
    }
    
    template class Option<int>;
    template class Option<char>;
    template class Option<size_t>;
};