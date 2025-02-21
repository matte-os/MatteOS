//
// Created by matejbucek on 29.10.22.
//

#pragma once

#include <Kernel/System/TrapFrame.h>
#include <Utils/Types.h>

namespace Kernel {
    class Thread {
        TrapFrame* m_trap_frame;
        uintptr_t* m_stack;
    public:
        Thread(TrapFrame* trap_frame, uintptr_t* stack) : m_trap_frame(trap_frame), m_stack(stack) {};
        TrapFrame* get_trap_frame() { return m_trap_frame; }
        ~Thread() = default;
    };
}