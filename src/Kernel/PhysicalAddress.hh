#pragma once

#include <Utils/Types.hh>

namespace Kernel{
    struct PhysicalAddress
    {
        u64 offset  : 12;
        u64 ppn0    : 9;
        u64 ppn1    : 9;
        u64 ppn2    : 26;
        u64 reserved: 8;

        public: 
            PhysicalAddress(){

            };

            PhysicalAddress(u64 value){
                *(u64*)this = value;
            };
    };
};