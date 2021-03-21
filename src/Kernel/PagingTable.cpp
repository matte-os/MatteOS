#include <Kernel/Paging.hh>
#include <Kernel/PagingTable.hh>
#include <Utils/Assertions.hh>
#include <Utils/Option.hh>

using Utils::ASSERT;
using Utils::Option;

namespace Kernel{
    bool TableEntry::isValid(){
        return entry & EntryBits::VALID != 0;
    }

    bool TableEntry::isInvalid(){
        return !isValid();
    }

    bool TableEntry::isLeaf(){
        return entry & 0xe != 0;
    }

    bool TableEntry::isBranch(){
        return !isLeaf();
    }

    void TableEntry::setEntry(__int64 entry){
        this->entry = entry;
    }

    __int64 TableEntry::getEntry(){
        return entry;
    }

    int PagingTable::length(){
        return 512;
    }

    void map(PagingTable &root, size_t vaddr, size_t paddr, __int64 bits, size_t level){
        ASSERT(bits & 0xe != 0);
        
        //Možná u32
        size_t vpn[] = {
            (vaddr >> 12) & 0x1ff,
            (vaddr >> 21) & 0x1ff,
            (vaddr >> 30) & 0x1ff
        };
        //Možná u32
        size_t ppn[] = {
            (paddr >> 12) & 0x1ff,
            (paddr >> 21) & 0x1ff,
            (paddr >> 30) & 0x3ffffff
        };

        TableEntry* v = &root.entries[vpn[2]];
        
        for(int i = 1; i <= level; i--){
            if(!v->isValid()){
                u8* page = Paging::zalloc(1);
                v->setEntry((((__int64)page) >> 2) | EntryBits::VALID);
            }
            TableEntry* entry = (TableEntry*)((v->getEntry() & !0x3ff) << 2);
            v = (TableEntry*)(entry + vpn[i]);
        }

        __int64 entry = ((__int64)(ppn[2] << 28)) |
                        ((__int64)(ppn[1] << 19)) |
                        ((__int64)(ppn[0] << 10)) |
                        bits |
                        EntryBits::VALID |
                        EntryBits::DIRTY |
                        EntryBits::ACCESS;
        v->setEntry(entry);

    }

    void umap(PagingTable &root){
        for(int i = 0; i < PagingTable::length(); i++){
            auto& lv2 = root.entries[i];
            if(lv2.isValid() && lv2.isBranch()){
                size_t memAddrLv1 = (lv2.getEntry() & !0x3ff) << 2;
                auto& tableLv1 = (PagingTable&) memAddrLv1;
                for(int j = 0; j < PagingTable::length(); j++){
                    auto& lv1 = tableLv1.entries[j];
                    if(lv1.isValid() && lv1.isBranch()){
                        size_t memAddrLv0 = (lv1.getEntry() & !0x3ff) << 2;
                        Paging::dealloc((u8*) memAddrLv0);
                    }
                }
                Paging::dealloc((u8*) memAddrLv1);
            }
        }
    }

    Option<size_t> virtualToPhysical(){
        return NULL;
    }
};