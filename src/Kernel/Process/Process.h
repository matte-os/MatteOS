//
// Created by matejbucek on 8.10.22.
//

#pragma once

#include <Kernel/System/TrapFrame.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/Process/Thread.h>
#include <Utils/Types.hh>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Pointers/RefPtr.h>

using Utils::ArrayList;
using Utils::Array;
using Utils::Pointers::RefPtr;
using Kernel::Memory::PageTable;
using Kernel::Memory::MemoryRegion;

namespace Kernel::Process {
    enum class ProcessState : u8 {
        RUNNING,
        SLEEPING,
        WAITING,
        DEAD
    };

    class Process {
    protected:
        size_t m_pid;
        size_t m_gid;
        Thread* m_thread;
        PageTable* m_page_table;
        ProcessState m_state;
    public:
        explicit Process(size_t pid,
                         Thread* thread,
                         PageTable* page_table,
                         ProcessState state) : m_pid(pid), m_gid(m_pid), m_thread(thread), m_page_table(page_table),
                                               m_state(state) {}
        virtual ~Process() = default;
        [[nodiscard]] ProcessState get_state() const { return m_state; }
        [[nodiscard]] Thread* get_thread() const { return m_thread; }
        void map_memory_region(MemoryRegion&);
        void unmap_memory_region(MemoryRegion&);
        [[nodiscard]] PageTable* get_page_table() const { return m_page_table; }
    };

    class KernelProcess : public Process {
    protected:
        RefPtr<Array<Thread*>> m_threads;
    public:
        explicit KernelProcess(size_t pid,
                               const RefPtr<Array<Thread*>>& threads,
                               PageTable* page_table,
                               ProcessState state) : Process(pid, nullptr, page_table, state) { m_threads = threads; }
        [[nodiscard]] RefPtr<Array<Thread*>> get_threads() const { return m_threads; }
    };

} // Kernel
