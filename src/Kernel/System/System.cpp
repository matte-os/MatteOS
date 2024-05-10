//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/CPU.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/System/System.h>
#include <Kernel/System/Trap.h>
#include <Kernel/sbi/sbi.hh>
#include <Utils/Basic.hh>
#include <Utils/DebugConsole.hh>

extern "C" void m_trap_vector();
extern "C" void switch_to_user(TrapFrame* trap_frame);

#define MY_OFFSETOF(type, member) ((size_t)(&((type *)0)->member))

namespace Kernel::System {
    static System* s_system;

    void System::init() {
        s_system = new System;
    }

    System& System::the() {
        return *s_system;
    }

    System::System() {
        m_memory_regions = RefPtr<ArrayList<MemoryRegion>>(new ArrayList<MemoryRegion>());
        m_number_of_harts = 0;
        init_harts();
        m_mtime = (u64*) 0x0200BFF8;
        //Utils::DebugConsole::println("System: Initialised.");
    }

    void System::parse_fdt(fdt_header* header) {
        if(__builtin_bswap32(header->magic) == 0xd00dfeed) {

            auto fdt_version = __builtin_bswap32(header->version);
            auto total_size = __builtin_bswap32(header->totalsize);

            //Parsing Reserved Memory Blocks
            auto memory_offset = __builtin_bswap32(header->off_mem_rsvmap);
            auto* memory_entry = (fdt_reserve_entry*) (((u8*)header) + memory_offset);
            Utils::DebugConsole::print_ln_number(memory_offset, 16);
            Utils::DebugConsole::print_ln_number(__builtin_bswap64(memory_entry->address), 16);
            Utils::DebugConsole::print_ln_number(__builtin_bswap64(memory_entry->size), 16);
            while(memory_entry->address != 0 && memory_entry->size != 0) {
                m_memory_regions->add(MemoryRegion(Memory::MemoryRegionType::CONVENTIONAL, __builtin_bswap64(memory_entry->address),
                                                  __builtin_bswap64(memory_entry->size), false));
                memory_entry++;
            }
            //Parsing Structure Blocks

        }
    }

    size_t System::get_number_of_harts() const {
        return m_number_of_harts;
    }

    void System::init_harts() {
        //FIXME: We should get this information out of a fdt. This is a fast hack to get it in dirty way.
        for(int i = 0; i < 100; i++) {
            auto hart = SBI::sbi_hart_get_status(i);
            if (hart.error == -3) {
                Utils::DebugConsole::print("System: sbi_hart_get_status returned ");
                Utils::DebugConsole::print_ln_number(hart.error, 10);
                break;
            } else {
                m_number_of_harts++;
            }
        }
        Utils::DebugConsole::print("System: The system has ");
        Utils::DebugConsole::print_number(m_number_of_harts, 10);
        Utils::DebugConsole::println(" harts.");
    }

    void System::set_trap_vector(void (trap_vector)()) {
        CPU::write_sstatus((0b01 << 11) | (1 << 7) | (1 << 5));
        CPU::write_stvec(trap_vector);
        CPU::write_sie(0x888);
    }

    void System::set_default_trap_vector() {
        set_trap_vector(m_trap_vector);
    }

    void System::setup_interrupts() {
        set_default_trap_vector();
    }

    void System::switch_to_user_mode(TrapFrame* frame_to_apply) {
        DebugConsole::println("About switching to usermode");
        DebugConsole::print_ln_number(MY_OFFSETOF(TrapFrame, program_counter), 10);
        DebugConsole::print_ln_number(frame_to_apply->program_counter, 16);
        switch_to_user(frame_to_apply);
    }
} // Kernel
