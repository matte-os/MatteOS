//
// Created by matejbucek on 31.8.22.
//

#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Basic.hh>

namespace Kernel::Memory {
    static MemoryManager* memoryManager;

    MemoryManager& MemoryManager::the() {
        return *memoryManager;
    }

    MemoryManager *MemoryManager::ptr() {
        return memoryManager;
    }

    void MemoryManager::init(size_t heapStart, size_t heapSize) {
        size_t numberOfPages = heapSize/PAGE_SIZE;
        size_t allocStart = alignValue(heapStart + (numberOfPages * sizeof(Page)), PAGE_ORDER);
        //The first page is allocated for the MemoryManager to live.
        memoryManager = (MemoryManager*)(allocStart);
        memoryManager->heapStart = heapStart;
        memoryManager->heapSize = heapSize;
        memoryManager->firstPage = (Page*) heapStart;
        memoryManager->firstPage[0].setStatus(PageStatus::LAST);
        memoryManager->numberOfPages = numberOfPages;
        for(int i = 1; i < numberOfPages; i++){
            memoryManager->firstPage[i].clear();
        }
        memoryManager->allocStart = allocStart;
    }

    uintptr_t* MemoryManager::alloc(size_t pages){
        for(int i = 0; i < numberOfPages; i++){
            bool found = false;
            if(firstPage[i].isFree()){
                found = true;
                for(int j = i; j < i + pages; j++){
                    if(!firstPage[j].isFree()){
                        found = false;
                        break;
                    }
                }
            }

            if(found){
                for(int k = i; k < i + pages - 1; k++){
                    firstPage[k].setStatus(PageStatus::TAKEN);
                }

                firstPage[i + pages - 1].setStatus(PageStatus::LAST);
                return (size_t*)(allocStart + PAGE_SIZE * i);
            }
        }

        return nullptr;
    }

    uintptr_t* MemoryManager::zalloc(size_t pages){
        size_t* allocated = alloc(pages);

        if(allocated == nullptr){
            return nullptr;
        }

        int size = (PAGE_SIZE * pages) / 8;
        u64* bigPtr = (u64*) allocated;
        for(int i = 0; i < size; i++){
            (*(bigPtr + i)) = 0;
        }

        return allocated;
    }

    void MemoryManager::dealloc(size_t* ptr){
        if(ptr == nullptr){
            return;
        }

        uintptr_t addr = heapStart + ((uintptr_t) ptr - allocStart) / PAGE_SIZE;
        Page* page = (Page*) addr;

        while(page->isTaken()){
            page->clear();
            page++;
        }

        page->clear();
    }

    void MemoryManager::debugOutput(){
        char buffer[64];

        DebugConsole::println("PageController: Debugging sequence started!");

        DebugConsole::print("Total number of pages: ");
        itoa(buffer, numberOfPages, 10);
        DebugConsole::println(buffer);

        int allocated = 0;
        for(int i = 0; i < numberOfPages; i++){
            if(!firstPage[i].isFree()){
                allocated++;
            }
        }

        DebugConsole::print("Allocated: ");
        itoa(buffer, allocated, 10);
        DebugConsole::println(buffer);

        DebugConsole::print("Free: ");
        itoa(buffer, numberOfPages - allocated, 10);
        DebugConsole::println(buffer);
    }

    void MemoryManager::mapRange(PageTable& root, size_t start, size_t end, u64 bits){
        size_t memAddr = start & ~(PAGE_SIZE - 1);
        size_t numKbPages = (alignValue(end, 12) - memAddr) / PAGE_SIZE;

        for(int i = 0; i < numKbPages; i++){
            map(root, *(VirtualAddress*) &memAddr, *(PhysicalAddress*) &memAddr, bits, 0);
            memAddr += 1 << 12;
        }
    }

    void MemoryManager::map(PageTable& root, VirtualAddress vaddr, PhysicalAddress paddr, size_t bits, int level){

        PageTableEntry* v = &root.entries[vaddr.vpn2];

        for(int i = 1; i >= level; i--){
            if(!v->isValid()){
                size_t* page = zalloc(1);
                PhysicalAddress pageAddress = (uintptr_t)page;
                v->valid = 1;
                v->ppn0 = pageAddress.ppn0;
                v->ppn1 = pageAddress.ppn1;
                v->ppn2 = pageAddress.ppn2;
            }

            auto* table = (PageTable*) ((v->getValue() & ~0x1ff) << 2);
            switch (i)
            {
                case 0:
                    v = &table->entries[vaddr.vpn0];
                    break;
                case 1:
                    v = &table->entries[vaddr.vpn1];
                    break;
            }
        }

        v->accessed = 1;
        v->dirty = 1;
        v->valid = 1;
        v->ppn0 = paddr.ppn0;
        v->ppn1 = paddr.ppn1;
        v->ppn2 = paddr.ppn2;
        v->setBits(bits);
    }

    size_t MemoryManager::alignValue(size_t value, size_t order){
        size_t o = (1 << order) - 1;
        return (value + o) & ~o;
    }

    MemoryManager::MemoryManager() {

    }

    MemoryManager::~MemoryManager() {

    }
}