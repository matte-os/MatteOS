//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/System/System.h>
#include <Kernel/CPU.hh>
#include <Utils/Arrays/Array.h>
#include <Utils/kmalloc.h>

namespace Kernel::Process {
    using System::System;
    using Memory::MemoryManager;
    using Utils::Array;
    static ProcessManager* s_process_manager;

    void ProcessManager::init(PageTable* kernel_root_table) {
        s_process_manager = new ProcessManager(kernel_root_table);
    }

    ProcessManager& ProcessManager::the() {
        return *s_process_manager;
    }

    ProcessManager::ProcessManager(PageTable* kernel_root_table) {
        m_processes = RefPtr<LinkedQueue<Process*>>(new LinkedQueue<Process*>());
        m_kernel_process = &initialize_kernel_process(kernel_root_table);
        m_pid_counter = 1;
        m_process_list_lock = SpinLock();
        DebugConsole::print("Depth: ");
        DebugConsole::print_ln_number(m_processes->size(), 10);
    }

    Process& ProcessManager::initialize_process() {

    }

    KernelProcess& ProcessManager::initialize_kernel_process(PageTable* kernel_root_table) {
        auto number_of_harts = System::the().get_number_of_harts();
        auto threads = RefPtr<Array<Thread*>>(new Array<Thread*>(number_of_harts));
        auto& memory_manager = MemoryManager::the();
        memory_manager.debug_output();
        for(size_t i = 0; i < number_of_harts; i++) {
            threads->set(i, initialize_thread(CPU::read_satp(), i));
        }
        DebugConsole::println("Test");
        m_kernel_process = new KernelProcess(0, threads, nullptr, kernel_root_table, ProcessState::RUNNING);
        return *m_kernel_process;
    }

    static void dummy_process_main() {
        DebugConsole::println("Hello from Dummy Process!");
        for(;;) {}
    }

    void ProcessManager::create_dummy_process() {
        DebugConsole::print("The dummy process PC: ");
        DebugConsole::print_ln_number(reinterpret_cast<size_t>(dummy_process_main), 16);
        m_processes->add(new Process(m_pid_counter++, initialize_thread(CPU::read_satp(), 0),
                                     reinterpret_cast<uintptr_t *>(&dummy_process_main), m_kernel_process->get_page_table(), ProcessState::RUNNING));
    }

    Thread *ProcessManager::initialize_thread(SATP satp, size_t hart_id) {
        auto* trap_frame = new TrapFrame;
        trap_frame->satp = satp;
        trap_frame->trap_stack = (u64*) ((u8*)MemoryManager::the().zalloc(STACK_PAGES)) + (STACK_PAGES * MemoryManager::PAGE_SIZE);
        trap_frame->hart_id = hart_id;
        return new Thread(trap_frame, (uintptr_t*) ((u8*)trap_frame->trap_stack) - (STACK_PAGES * MemoryManager::PAGE_SIZE));
    }
}