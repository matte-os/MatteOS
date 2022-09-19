//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>

namespace Kernel::Memory {
    static KernelMemoryAllocator* kernelMemoryAllocator;

    void KernelMemoryAllocator::init(uintptr_t* page) {
        kernelMemoryAllocator = new (page) KernelMemoryAllocator;
    }

    KernelMemoryAllocator& KernelMemoryAllocator::the() {
        return *kernelMemoryAllocator;
    }

    KernelMemoryAllocator::KernelMemoryAllocator() {
        head = (AllocHeader*) MemoryManager::the().zalloc(ALLOCATION_SIZE);
        totalSize = ALLOCATION_SIZE * MemoryManager::PAGE_SIZE;
        taken = 0;
        head->setFree();
        head->setSize(totalSize);
    }

    KernelMemoryAllocator::~KernelMemoryAllocator() {
        MemoryManager::the().dealloc((uintptr_t*) head);
    }

    uintptr_t* KernelMemoryAllocator::kmalloc(size_t sz) {
        auto size = MemoryManager::alignValue(sz, 3) + sizeof(AllocHeader);
        auto* ptr = head;
        auto* tail = (AllocHeader*)((u8*) head) + totalSize;
        while(ptr < tail) {
            if(ptr->isFree() && ptr->getSize() >= size) {
                auto chunkSize = ptr->getSize();
                auto remaining = chunkSize - size;
                ptr->setTaken();
                if(remaining > sizeof(AllocHeader)) {
                    auto* next = (AllocHeader*) (((u8*) ptr) + size);
                    next->setFree();
                    next->setSize(remaining);
                    ptr->setSize(size);
                } else {
                    ptr->setSize(chunkSize);
                }
                return (uintptr_t*) ptr + 1;
            } else {
                ptr = (AllocHeader*) (((u8*) ptr) + ptr->getSize());
            }
        }
        return nullptr;
    }

    uintptr_t* KernelMemoryAllocator::kzmalloc(size_t sz) {
        auto size = MemoryManager::alignValue(sz, 3);
        auto mem = kmalloc(size);

        for(int i = 0; i < size; i++) {
            *(((u8*)mem) + 1) = 0;
        }

        return mem;
    }

    void KernelMemoryAllocator::kfree(uintptr_t* ptr) {
        if(ptr != nullptr) {
            auto* alloc = ((AllocHeader*)ptr)-1;
            alloc->setFree();
            coalesce();
        }
    }

    void KernelMemoryAllocator::debug() {
        auto* ptr = head;
        auto* tail = (AllocHeader*)(((u8*) head) + totalSize);
        while (ptr < tail) {
            DebugConsole::print("Taken: ");
            DebugConsole::printNumber(ptr->isTaken(), 10);
            DebugConsole::print(", Size: ");
            DebugConsole::printLnNumber(ptr->getSize(), 10);
            ptr = (AllocHeader*) (((u8*) ptr) + ptr->getSize());
        }
    }

    void KernelMemoryAllocator::coalesce() {
        auto* ptr = head;
        auto* tail = (AllocHeader*)(((u8*) head) + totalSize);
        while (ptr < tail) {
            auto* next = (AllocHeader*) (((u8*) ptr) + ptr->getSize());
            if(ptr->getSize() == 0) {
                break;
            } else if(next >= tail) {
                break;
            } else if(ptr->isFree() && next->isFree()) {
                ptr->setSize(ptr->getSize() + next->getSize());
            }
            ptr = (AllocHeader*) (((u8*) ptr) + ptr->getSize());
        }
    }
}

namespace std { // NOLINT(cert-dcl58-cpp) These declarations must be in ::std and we are not using <new>
    struct nothrow_t {
        explicit nothrow_t() = default;
    };

    void terminate() {}

    extern const nothrow_t nothrow;

    enum class align_val_t : size_t {};
};

uintptr_t* kmalloc(size_t size) {
    return Kernel::Memory::KernelMemoryAllocator::the().kmalloc(size);
}

void kfree(uintptr_t* ptr) {
    Kernel::Memory::KernelMemoryAllocator::the().kfree(ptr);
}

void* operator new(size_t size) {
    return Kernel::Memory::KernelMemoryAllocator::the().kmalloc(size);
}

void* operator new[](size_t size) {
    return Kernel::Memory::KernelMemoryAllocator::the().kmalloc(size);
}

void operator delete(void* ptr) {
    return Kernel::Memory::KernelMemoryAllocator::the().kfree((uintptr_t*) ptr);
}

void operator delete[](void* ptr) {
    return Kernel::Memory::KernelMemoryAllocator::the().kfree((uintptr_t*) ptr);
}