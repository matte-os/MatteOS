#include <Kernel/VirtualMemory.hh>
#include <Kernel/Paging.hh>
#include <Utils/Assertions.hh>
#include <Utils/DebugConsole.hh>

using Utils::ASSERT;
using Utils::DebugConsole;

namespace Kernel{
    bool AllocList::isTaken(){
        return flagsSize & AllocListFlags::AL_TAKEN != 0;
    }

    bool AllocList::isFree(){
        return !isTaken();
    }

    void AllocList::setTaken(){
        this->flagsSize |= AllocListFlags::AL_TAKEN;
    }

    void AllocList::setFree(){
        this->flagsSize &= !AllocListFlags::AL_TAKEN;
    }

    void AllocList::setSize(size_t size){
        bool taken = isTaken();
        this->flagsSize = size & !AllocListFlags::AL_TAKEN;
        if(taken)
            setTaken();
    }

    size_t AllocList::getSize(){
        return flagsSize & !AllocListFlags::AL_TAKEN;
    }

    AllocList* VirtualMemory::KMEM_HEAD = nullptr;
    size_t VirtualMemory::KMEM_ALLOC = 0;
    PagingTable* VirtualMemory::KMEM_PAGE_TABLE = nullptr;


    void VirtualMemory::init(){
        u8* kAlloc = Paging::zalloc(64);
        ASSERT(kAlloc != nullptr);

        if(kAlloc == nullptr){
            DebugConsole::println("kAlloc = nullptr");
        }

        KMEM_ALLOC = 64;
        KMEM_HEAD = (AllocList*) kAlloc;
        KMEM_HEAD->setFree();
        KMEM_HEAD->setSize(KMEM_ALLOC * PAGE_SIZE);
        KMEM_PAGE_TABLE = (PagingTable*) Paging::zalloc(1);
    }

    PagingTable* VirtualMemory::getPagingTable(){
        return KMEM_PAGE_TABLE;
    }

    AllocList* VirtualMemory::getHead(){
        return KMEM_HEAD;
    }

    size_t VirtualMemory::getNumAllocations(){
        return KMEM_ALLOC;
    }

};