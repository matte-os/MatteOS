#include <Kernel/PageController.hh>
#include <Utils/DebugConsole.hh>
#include <Utils/Basic.hh>

using Utils::DebugConsole;

namespace Kernel{
    size_t PageController::heapStart;
    size_t PageController::heapSize;
    Page* PageController::firstPage;
    size_t PageController::numberOfPages;
    size_t PageController::allocStart;

    PageController::PageController(){

    }

    PageController::~PageController(){

    }

    void PageController::init(size_t heapStart, size_t heapSize){
        PageController::heapStart = heapStart;
        PageController::heapSize = heapSize;

        firstPage = (Page*) heapStart;
        numberOfPages = heapSize/PAGE_SIZE;
        for(int i = 0; i < numberOfPages; i++){
            firstPage[i].clear();
        }


        allocStart = alignValue(heapStart + (numberOfPages * sizeof(Page)), PAGE_ORDER);
    }

    size_t* PageController::alloc(size_t pages){
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

    size_t* PageController::zalloc(size_t pages){
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

    void PageController::dealloc(size_t* ptr){
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

    void PageController::debugOutput(){
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

    size_t PageController::alignValue(size_t value, size_t order){
        size_t o = (1 << order) - 1;
        return (value + o) & ~o;
    }

    void PageController::mapRange(PageTable& root, size_t start, size_t end, u64 bits){
        size_t memAddr = start & ~(PAGE_SIZE - 1);
        size_t numKbPages = (alignValue(end, 12) - memAddr) / PAGE_SIZE;

            DebugConsole::println("Seems to do sth: ");
            DebugConsole::printLnNumber(numKbPages, 10);
        for(int i = 0; i < numKbPages; i++){
            map(root, *(VirtualAddress*) &memAddr, *(PhysicalAddress*) &memAddr, bits, 0);
            memAddr += 1 << 12;
        }
    }

    void PageController::map(PageTable& root, VirtualAddress vaddr, PhysicalAddress paddr, size_t bits, int level){

        PageTableEntry* v = &root.entries[vaddr.vpn2];

        //DebugConsole::println("Mapping?");
        DebugConsole::printLnNumber(vaddr.offset, 2);
        DebugConsole::printLnNumber(vaddr.vpn0, 2);
        DebugConsole::printLnNumber(vaddr.vpn1, 2);
        DebugConsole::printLnNumber(vaddr.vpn2, 2);
        DebugConsole::printLnNumber(vaddr.reserved, 2);
        for(int i = 1; i >= level; i--){
            //DebugConsole::print("Is valid: ");
            //DebugConsole::printLnNumber(v->valid, 2);
            if(!v->isValid()){
                size_t* page = zalloc(1);
                PhysicalAddress pageAddress = (uintptr_t)page;
                DebugConsole::printLnNumber((uintptr_t)page, 2);
                v->valid = 1;
                v->ppn0 = pageAddress.ppn0;
                v->ppn1 = pageAddress.ppn1;
                v->ppn2 = pageAddress.ppn2;
            }

            PageTable* table = (PageTable*) ((v->getValue() & ~0x1ff) << 2); 
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
        v->read = 1;
        v->write = 1;
        v->execute = 1;
        //DebugConsole::printLnNumber(v->getValue(), 2);
    }

    void PageController::debugPageWalk(PageTable* root){
        debugPageWalkIdk(root, 0);
    }

    void PageController::debugPageWalkIdk(PageTable* root, int level){
        if(level == 3)
            return;
        for(int i = 0; i < 512; i++){
            if(root->entries[i].getValue() != 0){
                DebugConsole::print("Table lvl ");
                DebugConsole::printNumber(level, 10);
                DebugConsole::print(", entry: ");
                DebugConsole::printLnNumber(i, 2);
                DebugConsole::printLnNumber(((root->entries[i].getValue() & ~0x1ff) << 2), 2);
                debugPageWalkIdk((PageTable*)((root->entries[i].getValue() & ~0x1ff) << 2), ++level);
                DebugConsole::println("After debug");
                return;
            }
        }
    }

};