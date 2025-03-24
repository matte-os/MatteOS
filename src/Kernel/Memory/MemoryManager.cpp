//
// Created by matejbucek on 31.8.22.
//

#include <Kernel/Arch/riscv64/Satp.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Assertions.h>
#include <Utils/Basic.h>

namespace Kernel {
  extern "C" {
  char _data_start;
  char _data_end;
  char _rodata_start;
  char _rodata_end;
  char _text_start;
  char _text_end;
  char _bss_start;
  char _bss_end;
  char _memory_start;
  char _memory_end;
  char _heap_start;
  char _heap_size;
  char _stack_start;
  char _stack_end;
  char _context_switching_start;
  char _context_switching_end;
  char _text_special_start;
  char _text_special_end;
  }

  static MemoryManager* s_memory_manager = nullptr;

  MemoryManager& MemoryManager::the() {
    runtime_assert(s_memory_manager, "MemoryManager is not initialized!");
    return *s_memory_manager;
  }

  MemoryManager* MemoryManager::ptr() {
    return s_memory_manager;
  }

  void MemoryManager::init() {
    DebugConsole::print_ln_number(get_heap_start(), 16);
    DebugConsole::print_ln_number(get_heap_size(), 16);
    size_t numberOfPages = get_heap_size() / PAGE_SIZE;
    size_t allocStart = align_value(get_heap_start() + (numberOfPages * sizeof(Page)), PAGE_ORDER);
    //The first page is allocated for the MemoryManager to live.
    s_memory_manager = reinterpret_cast<MemoryManager*>(allocStart);
    s_memory_manager->m_heap_start = get_heap_start();
    s_memory_manager->m_heap_size = get_heap_size();
    s_memory_manager->m_first_page = (Page*) get_heap_start();
    s_memory_manager->m_first_page[0].set_status(PageStatus::Last);
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
          m_first_page[k].set_status(PageStatus::Taken);
        }
        m_first_page[i + pages - 1].set_status(PageStatus::Last);
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
  PageTable* MemoryManager::get_current_root_page_table() {
    SATP satp{};
    asm volatile("csrr %0, satp" : "=r"(satp));
    return reinterpret_cast<PageTable*>(satp.ppn << 12);
  }

  void MemoryManager::map_system_defaults(PageTable& root) {
    DebugConsole::println("MemoryManager: Mapping the heap.");
    identity_map_range(root, (uintptr_t) (&_heap_start),
                       ((uintptr_t) ((u64) &_heap_start + (u64) &_heap_size)),
                       (u64) EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the text section.");
    identity_map_range(root, (uintptr_t) &_text_start,
                       (uintptr_t) &_text_end, (u64) EntryBits::READ_EXECUTE);
    DebugConsole::println("MemoryManager: Mapping the rodata section.");
    identity_map_range(root, (uintptr_t) &_rodata_start,
                       (uintptr_t) &_rodata_end,
                       (u64) EntryBits::READ_EXECUTE);
    DebugConsole::println("MemoryManager: Mapping the data section.");
    identity_map_range(root, (uintptr_t) &_data_start,
                       (uintptr_t) &_data_end, (u64) EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the bss section.");
    identity_map_range(root, (uintptr_t) &_bss_start,
                       (uintptr_t) &_bss_end, (u64) EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the stack.");
    identity_map_range(root, (uintptr_t) &_stack_start,
                       (uintptr_t) &_stack_end, (u64) EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the UART.");
    map(root, 0x10000000, 0x10000000,
        (u64) EntryBits::READ_WRITE, 0);
    DebugConsole::println("MemoryManager: Mapping the CLINT.");
    identity_map_range(root, 0x02000000, 0x0200ffff,
                       (u64) EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the PLIC.");
    identity_map_range(root, 0x0c000000, 0x0c002001,
                       (u64) EntryBits::READ_WRITE);
    identity_map_range(root, 0x0c200000, 0x0c208001,
                       (u64) EntryBits::READ_WRITE);
    identity_map_range(root, (uintptr_t) &_context_switching_start, (uintptr_t) &_context_switching_end, (u64) EntryBits::READ_EXECUTE);
    identity_map_range(root, (uintptr_t) &_text_special_start, (uintptr_t) &_text_special_end, (u64) EntryBits::READ_EXECUTE);
  }

  uintptr_t MemoryManager::get_data_start() {
    return (uintptr_t) &_data_start;
  }
  uintptr_t MemoryManager::get_data_end() {
    return (uintptr_t) &_data_end;
  }
  uintptr_t MemoryManager::get_rodata_start() {
    return (uintptr_t) &_rodata_start;
  }
  uintptr_t MemoryManager::get_rodata_end() {
    return (uintptr_t) &_rodata_end;
  }
  uintptr_t MemoryManager::get_text_start() {
    return (uintptr_t) &_text_start;
  }
  uintptr_t MemoryManager::get_text_end() {
    return (uintptr_t) &_text_end;
  }
  uintptr_t MemoryManager::get_bss_start() {
    return (uintptr_t) &_bss_start;
  }
  uintptr_t MemoryManager::get_bss_end() {
    return (uintptr_t) &_bss_end;
  }
  uintptr_t MemoryManager::get_memory_start() {
    return (uintptr_t) &_memory_start;
  }
  uintptr_t MemoryManager::get_memory_end() {
    return (uintptr_t) &_memory_end;
  }
  uintptr_t MemoryManager::get_heap_start() {
    return (uintptr_t) &_heap_start;
  }
  uintptr_t MemoryManager::get_heap_size() {
    return (uintptr_t) &_heap_size;
  }
  uintptr_t MemoryManager::get_stack_start() {
    return (uintptr_t) &_stack_start;
  }
  uintptr_t MemoryManager::get_stack_end() {
    return (uintptr_t) &_stack_end;
  }
  uintptr_t MemoryManager::get_context_switching_start() {
    return (uintptr_t) &_context_switching_start;
  }
  uintptr_t MemoryManager::get_context_switching_end() {
    return (uintptr_t) &_context_switching_end;
  }
  uintptr_t MemoryManager::get_text_special_start() {
    return (uintptr_t) &_text_special_start;
  }
  uintptr_t MemoryManager::get_text_special_end() {
    return (uintptr_t) &_text_special_end;
  }
}// namespace Kernel