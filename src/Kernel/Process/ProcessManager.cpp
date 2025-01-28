#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/System/System.h>
#include <Utils/Arrays/Array.h>
#include <Utils/kmalloc.h>
#include <Kernel/Process/Console/Console.h>

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
        if (fd != -1) {
            syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Opened file successfully"));
        } else {
            syscall(Syscalls::Sys$dbgln, reinterpret_cast<uintptr_t>("Failed to open file"));
        }

        u32 time = 0;
        for (;;) {
            if (time == 1000000000) {
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

        DebugConsole::print("ProcessManager: The size of the process queue is: ");
        DebugConsole::print_ln_number(m_processes->size(), 10);
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

        auto satp = CPU::build_satp(SatpMode::Sv39, 1, (uintptr_t) root);

        DebugConsole::print("The dummy process PC: ");
        DebugConsole::print_ln_number(program_counter,
                                      16);
        auto* thread = initialize_thread(
            program_counter, satp, 0);
        auto* trap_frame = thread->get_trap_frame();

        // Map the kernel trap frame to the special address
        MemoryManager::the().map(
            *root, THREAD_FRAME_ADDRESS,
            reinterpret_cast<uintptr_t>(trap_frame),
            (size_t) EntryBits::READ_WRITE, 0);

        // Map the stack
        MemoryManager::the().map_range(*root, STACK_ADDRESS - 0x1000,
                                       STACK_ADDRESS,
                                       reinterpret_cast<uintptr_t>(trap_frame),
                                       (size_t) EntryBits::USER_READ_WRITE);

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
} // namespace Kernel
