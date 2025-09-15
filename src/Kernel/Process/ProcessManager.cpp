/**
 * @file ProcessManager.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/Console/Console.h>
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
    m_kernel_process = &initialize_kernel_process(kernel_root_table);
    m_pid_counter = 1;
  }

  Process& ProcessManager::initialize_process() {
  }

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
      if(time == 1000000000) {
        syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Process 1 is alive!"));
        time = 0;
        continue;
      }

      time++;
    }
  }

  void ProcessManager::create_initial_processes() {
    // Dummy main
    auto* root = create_dummy_process(reinterpret_cast<uintptr_t>(dummy_process_main));

    // Console main
    auto* root2 = create_dummy_process(reinterpret_cast<uintptr_t>(console_main));
  }

  PageTable* ProcessManager::create_dummy_process(uintptr_t program_counter) {
    auto* root = reinterpret_cast<PageTable*>(MemoryManager::the().zalloc(1));
    MemoryManager::the().identity_map_range(*root, MemoryManager::get_text_start(), MemoryManager::get_text_end(),
                                            (u64) (EntryBits::USER_READ_EXECUTE));
    MemoryManager::the().identity_map_range(*root, MemoryManager::get_text_special_start(),
                                            MemoryManager::get_text_special_end(),
                                            (u64) (EntryBits::USER_READ_EXECUTE));

    MemoryManager::the().identity_map_range(*root, MemoryManager::get_context_switching_start(),
                                            MemoryManager::get_context_switching_end(),
                                            static_cast<u64>(EntryBits::READ_EXECUTE));

    MemoryManager::the().identity_map_range(*root, MemoryManager::get_data_start(), MemoryManager::get_data_end(), (u64) (EntryBits::USER_READ_WRITE));
    MemoryManager::the().identity_map_range(*root, MemoryManager::get_bss_start(), MemoryManager::get_bss_end(), (u64) (EntryBits::USER_READ_WRITE));
    MemoryManager::the().identity_map_range(*root, MemoryManager::get_rodata_start(), MemoryManager::get_rodata_end(), (u64) (EntryBits::USER_READ));


    auto satp = CPU::build_satp(SatpMode::Sv39, 1, (uintptr_t) root);

    auto current_pid = m_pid_counter++;

    auto* thread = initialize_thread(
            program_counter, satp, 0, current_pid, 0);
    auto* trap_frame = thread->get_trap_frame();

    // Map the kernel trap frame to the special address
    MemoryManager::the().map(
            *root, THREAD_FRAME_ADDRESS,
            reinterpret_cast<uintptr_t>(trap_frame),
            (size_t) EntryBits::READ_WRITE, 0);

    // Map the stack
    MemoryManager::the().map_range(*root, STACK_ADDRESS - 0x1000,
                                   STACK_ADDRESS,
                                   reinterpret_cast<uintptr_t>(trap_frame->trap_stack),
                                   (size_t) EntryBits::USER_READ_WRITE);


    auto* process = new Process(current_pid, thread,
                                root,
                                ProcessState::Running);
    m_process_list.add(process);
    m_run_queue.append(thread);
    return root;
  }

  Thread* ProcessManager::initialize_thread(uintptr_t program_counter, SATP satp,
                                            size_t hart_id, size_t pid, size_t tid) {
    auto* trap_frame = new(MemoryManager::the().zalloc(1)) TrapFrame;
    trap_frame->satp = satp;
    trap_frame->trap_stack =
            (u64*) ((u8*) MemoryManager::the().zalloc(STACK_PAGES)) +
            (STACK_PAGES * MemoryManager::PAGE_SIZE);
    trap_frame->regs[2] = STACK_ADDRESS;
    trap_frame->program_counter = program_counter;
    return new Thread(trap_frame, (uintptr_t*) ((u8*) trap_frame->trap_stack) -
                                          (STACK_PAGES * MemoryManager::PAGE_SIZE), pid, tid);
  }

  ProcessManager::~ProcessManager() {
    DebugConsole::println("ProcessManager: destroyed");
  }

  ErrorOr<Process*> ProcessManager::get_process(u64 pid) {
    return m_process_list.find_first_match([pid](auto* process) {
      return process->m_pid == pid;
    });
  }

  ErrorOr<Thread*> ProcessManager::get_thread(u64 pid, u64 tid) {
    auto process = TRY(get_process(pid));
    return process->get_threads().find_first_match([tid](auto* thread) {
      return thread->m_tid == tid;
    });
  }

  void ProcessManager::unblock(size_t pid, size_t tid) {
    auto error_or_thread = get_thread(pid, tid);
    if(error_or_thread.has_error()) {
      return;
    }

    auto thread = error_or_thread.get_value();
    thread->m_state = ThreadState::Running;

    //TODO: m_run_queue.add(thread);
  }

  void ProcessManager::block(size_t pid, size_t tid) {
    auto error_or_thread = get_thread(pid, tid);
    if(error_or_thread.has_error()) {
      return;
    }

    auto thread = error_or_thread.get_value();
    thread->m_state = ThreadState::Blocked;
    //TODO: m_run_queue.remove(thread);
  }

  Thread* ProcessManager::get_current_thread() {
    auto* trap_frame = System::get_current_kernel_trap_frame();
    auto error_or_thread = get_thread(trap_frame->current_process_id, trap_frame->current_thread_id);

    if(error_or_thread.has_error()) {
      return nullptr;
    }

    return error_or_thread.get_value();
  }

  Process* ProcessManager::get_current_process() {
    auto* trap_frame = System::get_current_kernel_trap_frame();
    auto error_or_process = get_process(trap_frame->current_process_id);

    if(error_or_process.has_error()) {
      return nullptr;
    }

    return error_or_process.get_value();
  }
}// namespace Kernel
