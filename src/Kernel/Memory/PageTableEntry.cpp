#include <Kernel/Memory/PageTableEntry.h>

namespace Kernel::Memory{
    u64 PageTableEntry::get_value(){
        u64 out = *(u64*)this;
        return out;
    }

    void PageTableEntry::set_value(u64 value){
        *(u64*)this = value;
    }

    void PageTableEntry::set_bits(u64 bits){
        set_value(get_value() | bits);
    }

    bool PageTableEntry::is_valid(){
        return valid == 1;
    }
};