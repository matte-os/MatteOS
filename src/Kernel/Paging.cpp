#include <Kernel/Paging.hh>
#include <Utils/Assertions.hh>
#include <Utils/DebugConsole.hh>
#include <Utils/Basic.hh>

using Utils::ASSERT;
using Utils::DebugConsole;

namespace Kernel{
    void Paging::init(size_t start, size_t size){
        HEAP_START = start;
        HEAP_SIZE = size;
        int numPages = getHeapSize()/PAGE_SIZE;
        Page* ptr = (Page*) getHeapStart();
        for (int i = 0; i < numPages; i++)
            ptr[i].clear();
        
        ALLOC_START =  alignValue(getHeapStart() + numPages * sizeof(Page), PAGE_ORDER); 
    }

    u8*  Paging::alloc(size_t pages){
        ASSERT(pages > 0);

        int numPages = getHeapSize()/PAGE_SIZE;

        Page* ptr = (Page*) getHeapStart();
        for(int i = 0; i < numPages; i++){
            bool found;
            if(ptr[i].isFree()){
                DebugConsole::println("There is some free mem");
                found = true;
                for(int j = i; j < i + pages; j++){
                    if(ptr[j].isTaken()){
                        found = false;
                        break;
                    }
                }
            }

            if(found){
                for (int k = i; k < i + pages - 1; k++){
                    ptr[k].setFlag(PageBits::TAKEN);
                }

                ptr[i + pages - 1].setFlag(PageBits::TAKEN);
                ptr[i + pages - 1].setFlag(PageBits::LAST);

                return (u8*)(ALLOC_START + PAGE_SIZE * i);
            }
        }

        return nullptr;
    }

    u8* Paging::zalloc(size_t pages){
        u8* ret = alloc(pages);
        if(ret != nullptr){
            int size = (PAGE_SIZE * pages) / 8;
            u64* bigPtr = (u64*) ret;
            for(int i = 0; i < size; i++){
                (*(bigPtr + i)) = 0;
            }
        }
        return ret;
    }

    void Paging::dealloc(u8* ptr){
        ASSERT(ptr != nullptr);

        size_t addr = getHeapStart() + ((size_t) ptr - ALLOC_START) / PAGE_SIZE;
        ASSERT(addr >= getHeapStart() && addr < getHeapStart() + getHeapSize());
        Page* p = (Page*)addr;

        while(p->isTaken() && !p->isLast()){
            p->clear();
            p += 1;
        } 

        ASSERT(p->isLast() == true);

        p->clear();
    }
};