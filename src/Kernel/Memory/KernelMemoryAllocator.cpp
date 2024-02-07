//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>

namespace Kernel::Memory {
    static KernelMemoryAllocator* s_kernel_memory_allocator;

    void KernelMemoryAllocator::init(uintptr_t* page) {
        s_kernel_memory_allocator = new (page) KernelMemoryAllocator;
    }

    KernelMemoryAllocator& KernelMemoryAllocator::the() {
        return *s_kernel_memory_allocator;
    }

    KernelMemoryAllocator::KernelMemoryAllocator() {
        m_head = (AllocHeader*) MemoryManager::the().zalloc(ALLOCATION_SIZE);
        m_total_size = ALLOCATION_SIZE * MemoryManager::PAGE_SIZE;
        m_taken = 0;
        m_head->set_free();
        m_head->set_size(m_total_size);
    }

    KernelMemoryAllocator::~KernelMemoryAllocator() {
        MemoryManager::the().dealloc((uintptr_t*) m_head);
    }

    uintptr_t* KernelMemoryAllocator::kmalloc(size_t sz) {
        auto size = MemoryManager::align_value(sz, 3) + sizeof(AllocHeader);
        auto* ptr = m_head;
        auto* tail = (AllocHeader*)((u8*) m_head) + m_total_size;
        while(ptr < tail) {
            if(ptr->is_free() && ptr->get_size() >= size) {
                auto chunkSize = ptr->get_size();
                auto remaining = chunkSize - size;
                ptr->set_taken();
                if(remaining > sizeof(AllocHeader)) {
                    auto* next = (AllocHeader*) (((u8*) ptr) + size);
                    next->set_free();
                    next->set_size(remaining);
                    ptr->set_size(size);
                } else {
                    ptr->set_size(chunkSize);
                }
                return (uintptr_t*) ptr + 1;
            } else {
                ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
            }
        }
        return nullptr;
    }

    uintptr_t* KernelMemoryAllocator::kzmalloc(size_t sz) {
        auto size = MemoryManager::align_value(sz, 3);
        auto mem = kmalloc(size);

        for(size_t i = 0; i < size; i++) {
            *(((u8*)mem) + 1) = 0;
        }

        return mem;
    }

    void KernelMemoryAllocator::kfree(uintptr_t* ptr) {
        if(ptr != nullptr) {
            auto* alloc = ((AllocHeader*)ptr)-1;
            alloc->set_free();
            coalesce();
        }
    }

    void KernelMemoryAllocator::debug() {
        auto* ptr = m_head;
        auto* tail = (AllocHeader*)(((u8*) m_head) + m_total_size);
        while (ptr < tail) {
            DebugConsole::print("Taken: ");
            DebugConsole::print_number(ptr->is_taken(), 10);
            DebugConsole::print(", Size: ");
            DebugConsole::print_ln_number(ptr->get_size(), 10);
            ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
        }
    }

    void KernelMemoryAllocator::coalesce() {
        auto* ptr = m_head;
        auto* tail = (AllocHeader*)(((u8*) m_head) + m_total_size);
        while (ptr < tail) {
            auto* next = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
            if(ptr->get_size() == 0) {
                break;
            } else if(next >= tail) {
                break;
            } else if(ptr->is_free() && next->is_free()) {
                ptr->set_size(ptr->get_size() + next->get_size());
            }
            ptr = (AllocHeader*) (((u8*) ptr) + ptr->get_size());
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