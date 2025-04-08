//
// Created by matejbucek on 13.5.24.
//

#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/SBI/SBI.h>
#include <Kernel/System/Timer.h>
#include <Utils/Assertions.h>

namespace Kernel {
    static Timer* s_timer = nullptr;

    void Timer::init() {
        if (s_timer == nullptr) {
            const auto error_or_cpus = DeviceTree::the().find_node("/cpus");
            runtime_assert(error_or_cpus.has_value(), "Cpus not found in device tree");
            const auto cpus = error_or_cpus.get_value();

            const auto error_or_frequency = cpus->find_property("timebase-frequency");
            runtime_assert(error_or_frequency.has_value(), "Timebase frequency not found in device tree");

            const auto frequency = error_or_frequency.get_value();
            s_timer = new Timer(frequency->get_value_as_u32());
        }
    }

    Timer& Timer::the() {
        runtime_assert(s_timer, "Timer is not initialized");
        return *s_timer;
    }

    void Timer::set_timer(const u64 time_in_ms) const {
        const auto current_time = RISCV64::CSR::read<RISCV64::CSR::Address::TIME>();

        const u64 ticks = (m_frequency / 1000) * time_in_ms;
        const u64 next_time = current_time + ticks;

        SBI::sbi_set_timer(next_time);
    }
} // namespace Kernel
