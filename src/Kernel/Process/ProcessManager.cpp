#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/System/System.h>
#include <Utils/Arrays/Array.h>
#include <Utils/kmalloc.h>

namespace Kernel {
  using Kernel::MemoryManager;
  using Kernel::System;
  using Utils::Array;
  static ProcessManager* s_process_manager;

  void ProcessManager::init(PageTable* kernel_root_table) {
    s_process_manager = new ProcessManager(kernel_root_table);
    DebugConsole::println("ProcessManager: Constructed");
  }

  ProcessManager& ProcessManager::the() { return *s_process_manager; }

  ProcessManager::ProcessManager(PageTable* kernel_root_table)
      : m_process_list_lock(SpinLock()) {
    m_processes = RefPtr(new LinkedQueue<Process*>());
    m_kernel_process = &initialize_kernel_process(kernel_root_table);
    m_pid_counter = 1;
    DebugConsole::print("Depth: ");
    DebugConsole::print_ln_number(m_processes->size(), 10);
  }

  Process& ProcessManager::initialize_process() {}

  KernelProcess&
  ProcessManager::initialize_kernel_process(PageTable* kernel_root_table) {
    m_kernel_process =
            new KernelProcess(0, kernel_root_table, ProcessState::Running);
    return *m_kernel_process;
  }

  extern "C" [[noreturn]] void dummy_process_main() {
    syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Hello from dummy process!"));
    auto fd = static_cast<int>(syscall(Syscalls::Sys$open, reinterpret_cast<uintptr_t>("/hello.txt"), 0));
    if(fd != -1) {
      syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Opened file successfully"));
    } else {
      syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Failed to open file"));
    }

    u32 time = 0;
    for(;;) {
      if (time == 1000000000) {
        syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Process 1 is alive!"));
        time = 0;
        continue;
      }

      time++;
    }
  }

  PageTable* ProcessManager::create_dummy_process(uintptr_t text_start,
                                                  uintptr_t text_end) {
    auto* root = reinterpret_cast<PageTable*>(MemoryManager::the().zalloc(1));
    DebugConsole::print("Text start: ");
    DebugConsole::print_ln_number(text_start, 16);
    DebugConsole::print("Text end: ");
    DebugConsole::print_ln_number(text_end, 16);
    DebugConsole::print("Context switching start: ");
    DebugConsole::print_ln_number(MemoryManager::get_context_switching_start(), 16);
    DebugConsole::print("Context switching end: ");
    DebugConsole::print_ln_number(MemoryManager::get_context_switching_end(), 16);

    MemoryManager::the().identity_map_range(*root, text_start, text_end,
                                            (u64) (Kernel::EntryBits::USER_READ_EXECUTE));
    MemoryManager::the().identity_map_range(*root, MemoryManager::get_text_special_start(),
                                            MemoryManager::get_text_special_end(),
                                            (u64) (Kernel::EntryBits::USER_READ_EXECUTE));
    auto satp = Kernel::CPU::build_satp(Kernel::SatpMode::Sv39, 1, (uintptr_t) root);

    DebugConsole::print("The dummy process PC: ");
    DebugConsole::print_ln_number(reinterpret_cast<size_t>(dummy_process_main),
                                  16);
    auto* thread = initialize_thread(
            reinterpret_cast<uintptr_t>(dummy_process_main), satp, 0);
    auto* trap_frame = thread->get_trap_frame();

    // Map the kernel trap frame to the special address
    MemoryManager::the().map(
            *root, ProcessManager::THREAD_FRAME_ADDRESS,
            reinterpret_cast<uintptr_t>(trap_frame),
            (size_t) Kernel::EntryBits::READ_WRITE, 0);

    // Map the stack
    MemoryManager::the().map_range(*root, ProcessManager::STACK_ADDRESS - 0x1000,
                                   ProcessManager::STACK_ADDRESS,
                                   reinterpret_cast<uintptr_t>(trap_frame),
                                   (size_t) Kernel::EntryBits::USER_READ_WRITE);

    m_processes->add(new Process(m_pid_counter++, thread,
                                 m_kernel_process->get_page_table(),
                                 ProcessState::Running));
    return root;
  }

  Thread* ProcessManager::initialize_thread(uintptr_t program_counter, SATP satp,
                                            size_t hart_id) {
    auto* trap_frame = new(MemoryManager::the().zalloc(1)) TrapFrame;
    trap_frame->satp = satp;
    trap_frame->trap_stack =
            (u64*) ((u8*) MemoryManager::the().zalloc(STACK_PAGES)) +
            (STACK_PAGES * MemoryManager::PAGE_SIZE);
    trap_frame->regs[2] = STACK_ADDRESS;
    trap_frame->program_counter = program_counter;
    return new Thread(trap_frame, (uintptr_t*) ((u8*) trap_frame->trap_stack) -
                                          (STACK_PAGES * MemoryManager::PAGE_SIZE));
  }

  ProcessManager::~ProcessManager() {
    DebugConsole::println("ProcessManager destroyed");
  }
}// namespace Kernel
