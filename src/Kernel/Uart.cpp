#include <Kernel/Uart.hh>
namespace Kernel{
    Uart::Uart(size_t baseAddress){
        this->baseAddress = baseAddress;
    }

    void Uart::init(){
        u8* ptr = (u8*) baseAddress;
        *(ptr + 3) = 0b11;
        *(ptr + 2) = 1;
        *(ptr + 1) = 1; 
    }

    void Uart::put(const char c){
        u8* ptr = (u8*) baseAddress;
        *(ptr + 0) = c;
    }

    Utils::Option<char> get(){
        u8* ptr = (u8*) 0;
        if((*(ptr + 5)) & 1 == 0){
            return Utils::Option<char>();
        }else{
            return Utils::Option<char>(*(ptr + 0));
        }
    }
};
    