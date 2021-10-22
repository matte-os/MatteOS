#include <Kernel/PageTableEntry.hh>

namespace Kernel{
    u64 PageTableEntry::getValue(){
        u64 out = *(u64*)this;
        return out;
    }

    void PageTableEntry::setValue(u64 value){
        *(u64*)this = value;
    }

    void PageTableEntry::setBits(u64 bits){
        setValue(getValue()|bits);
    }

    bool PageTableEntry::isValid(){
        return valid == 1;
    }
};