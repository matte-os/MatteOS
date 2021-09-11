#pragma once

#include <Utils/Types.hh>

namespace Kernel{
    struct PageTableEntry{
        u64 valid   : 1; //Page is valid
        u64 read    : 1; //Page is readable
        u64 write   : 1; //Page is writable
        u64 execute : 1; //Page is executable
        u64 user    : 1; //Page is userspace
        u64 global  : 1; //Page is globalspace
        u64 accessed: 1; //Page was read/written to since last A = 0
        u64 dirty   : 1; //Page was written to since last D = 0
        u64 rsw     : 2; //The RSW field is reserved for use by supervisor software
        u64 ppn0    : 9; //First phisical page number
        u64 ppn1    : 9; //Second -||-
        u64 ppn2    : 26;//Third -||-
        u64 reserved: 10;//Reserved for future use

        //void setRWE();
        //void setRW();
        u64 getValue();
        void setValue(u64);
        bool isValid();
    };
};