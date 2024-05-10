//
// Created by matejbucek on 31.8.22.
//

#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Basic.hh>

namespace Kernel::Memory {
  static MemoryManager* s_memory_manager;

  MemoryManager& MemoryManager::the() {
    return *s_memory_manager;
  }

  MemoryManager* MemoryManager::ptr() {
    return s_memory_manager;
  }

  void MemoryManager::init(size_t heapStart, size_t heapSize) {
    size_t numberOfPages = heapSize / PAGE_SIZE;
    size_t allocStart = align_value(heapStart + (numberOfPages * sizeof(Page)), PAGE_ORDER);
    //The first page is allocated for the MemoryManager to live.
    s_memory_manager = (MemoryManager*) (allocStart);
    s_memory_manager->m_heap_start = heapStart;
    s_memory_manager->m_heap_size = heapSize;
    s_memory_manager->m_first_page = (Page*) heapStart;
    s_memory_manager->m_first_page[0].set_status(PageStatus::LAST);
    s_memory_manager->m_number_of_pages = numberOfPages;
    for(size_t i = 1; i < numberOfPages; i++) {
      s_memory_manager->m_first_page[i].clear();
    }
    s_memory_manager->m_alloc_start = allocStart;
  }

  uintptr_t* MemoryManager::alloc(size_t pages) {
    for(size_t i = 0; i < m_number_of_pages; i++) {
      bool found = false;
      if(m_first_page[i].is_free()) {
        found = true;
        for(size_t j = i; j < i + pages; j++) {
          if(!m_first_page[j].is_free()) {
            found = false;
            break;
          }
        }
      }

      if(found) {
        for(size_t k = i; k < i + pages - 1; k++) {
          m_first_page[k].set_status(PageStatus::TAKEN);
        }
        m_first_page[i + pages - 1].set_status(PageStatus::LAST);
        return (size_t*) (m_alloc_start + PAGE_SIZE * i);
      }
    }
    return nullptr;
  }

  uintptr_t* MemoryManager::zalloc(size_t pages) {
    size_t* allocated = alloc(pages);

    if(allocated == nullptr) {
      return nullptr;
    }
    size_t size = (PAGE_SIZE * pages) / 8;
    u64* bigPtr = (u64*) allocated;
    for(size_t i = 0; i < size; i++) {
      (*(bigPtr + i)) = 0;
    }
    return allocated;
  }

  void MemoryManager::dealloc(size_t* ptr) {
    if(ptr == nullptr) {
      return;
    }

    uintptr_t addr = m_heap_start + ((uintptr_t) ptr - m_alloc_start) / PAGE_SIZE;
    Page* page = (Page*) addr;

    while(page->is_taken()) {
      page->clear();
      page++;
    }

    page->clear();
  }

  void MemoryManager::debug_output() {
    char buffer[64];

    DebugConsole::println("PageController: Debugging sequence started!");

    DebugConsole::print("Total number of pages: ");
    itoa(buffer, m_number_of_pages, 10);
    DebugConsole::println(buffer);

    int allocated = 0;
    for(size_t i = 0; i < m_number_of_pages; i++) {
      if(!m_first_page[i].is_free()) {
        allocated++;
      }
    }

    DebugConsole::print("Allocated: ");
    itoa(buffer, allocated, 10);
    DebugConsole::println(buffer);

    DebugConsole::print("Free: ");
    itoa(buffer, m_number_of_pages - allocated, 10);
    DebugConsole::println(buffer);
  }

  void MemoryManager::identity_map_range(PageTable& root, size_t start, size_t end, u64 bits) {
    size_t memAddr = start & ~(PAGE_SIZE - 1);
    size_t numKbPages = (align_value(end, 12) - memAddr) / PAGE_SIZE;

    for(size_t i = 0; i < numKbPages; i++) {
      map(root, *(VirtualAddress*) &memAddr, *(PhysicalAddress*) &memAddr, bits, 0);
      memAddr += 1 << 12;
    }
  }

  void MemoryManager::map(PageTable& root, VirtualAddress vaddr, PhysicalAddress paddr, size_t bits, int level) {

    PageTableEntry* v = &root.m_entries[vaddr.vpn2];

    for(int i = 1; i >= level; i--) {
      if(!v->is_valid()) {
        size_t* page = zalloc(1);
        PhysicalAddress pageAddress = (uintptr_t) page;
        v->valid = 1;
        v->ppn0 = pageAddress.ppn0;
        v->ppn1 = pageAddress.ppn1;
        v->ppn2 = pageAddress.ppn2;
      }

      auto* table = (PageTable*) ((v->get_value() & ~0x1ff) << 2);
      v = &table->m_entries[(i == 0) ? vaddr.vpn0 : vaddr.vpn1];
    }

    v->accessed = 1;
    v->dirty = 1;
    v->valid = 1;
    v->ppn0 = paddr.ppn0;
    v->ppn1 = paddr.ppn1;
    v->ppn2 = paddr.ppn2;
    v->set_bits(bits);
  }

  size_t MemoryManager::align_value(size_t value, size_t order) {
    size_t o = (1 << order) - 1;
    return (value + o) & ~o;
  }

  MemoryManager::MemoryManager() {
  }

  MemoryManager::~MemoryManager() {
  }
  void MemoryManager::map_range(PageTable& root, size_t virtual_start, size_t virtual_end, size_t physical_start, u64 bits) {
    size_t virtual_memory_address = virtual_start & ~(PAGE_SIZE - 1);
    size_t physical_memory_address = physical_start & ~(PAGE_SIZE - 1);
    size_t number_of_pages = (align_value(virtual_end, 12) - virtual_memory_address) / PAGE_SIZE;

    for(size_t i = 0; i < number_of_pages; i++) {
      map(root, *(VirtualAddress*) &virtual_memory_address, *(PhysicalAddress*) &physical_memory_address, bits, 0);
      virtual_memory_address += 1 << 12;
      physical_memory_address += 1 << 12;
    }
  }
}// namespace Kernel::Memory