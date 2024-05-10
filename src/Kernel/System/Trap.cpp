#include <Kernel/System/Plic.h>
#include <Kernel/System/Trap.h>
#include <Kernel/Uart.h>
#include <Utils/DebugConsole.hh>

using Kernel::System::Plic;
using Kernel::TrapFrame;
using Kernel::Uart;

extern "C" size_t trap_vector(
    u64 sepc,
    u64 stval,
    u64 scause,
    u64 cpu_id,
    u64 sstatus)
{
    bool async = (scause >> 63 & 1) == 1;

    //Utils::DebugConsole::printLnNumber(*(u64*)&frame->satp, 16);

    size_t causeNum = scause & 0xfff;
    size_t returnEpc = sepc;

    //Utils::DebugConsole::println("Trap");
    //Utils::DebugConsole::printLnNumber(cause, 16);

    if (async) {
        switch (causeNum)
        {
        case 3:
        {
            Utils::DebugConsole::println("Software interrupt");
            break;
        }
        case 7:
        {
            //FIXME: We should use the System::set_timer function instead.
            u64 *mTimeCmp = (u64 *)0x02004000;
            u64 *mTime = (u64 *)0x0200bff8;
            *mTimeCmp = *mTime + 10000000;
            break;
        }
        case 11:
        {
            //Utils::DebugConsole::println("Machine external interrupt.");
            auto next = Plic::the().next();
            if (next.has_some())
            {
                switch (next.get_value())
                {
                //UART
                case 10:
                {
                    Uart uart = Uart(0x10000000);
                    auto value = uart.get();
                    if (value.has_some())
                    {
                        switch (value.get_value())
                        {
                        case 8:
                        {
                            Utils::DebugConsole::print(" ");
                            break;
                        }
                        case 10 | 13:
                        {
                            Utils::DebugConsole::println("");
                            break;
                        }
                        default:
                        {
                            Utils::DebugConsole::print(value.get_value());
                            break;
                        }
                        }
                    }
                    break;
                }

                default:
                {
                    Utils::DebugConsole::println("Non-UART external interrupt");
                    break;
                }
                }
                Plic::the().complete(next.get_value());
            }
            break;
        }
        default:
        {
            Utils::DebugConsole::println("Unhandled async trap.");
            break;
        }
        }
    }
    else
    {
        switch (causeNum)
        {
        case 2:
        {
            Utils::DebugConsole::println("Illegal instruction.");
            break;
        }
        case 8:
        {
            Utils::DebugConsole::println("Ecall from User mode!");
            returnEpc += 4;
            break;
        }
        case 9:
        {
            Utils::DebugConsole::println("Ecall from Supervisor mode!");
            returnEpc += 4;
            break;
        }
        case 11:
        {
            Utils::DebugConsole::println("ECall from Machine mode!");
            break;
        }
        case 12:
        {
            Utils::DebugConsole::println("Instruction page fault!");
            returnEpc += 4;
            break;
        }
        case 13:
        {
            Utils::DebugConsole::println("Load page fault!");
            returnEpc += 4;
            break;
        }
        case 15:
        {
            Utils::DebugConsole::println("Store page fault!");
            returnEpc += 4;
            break;
        }
        default:
        {
            Utils::DebugConsole::println("Unhandled sync trap.");
        }
        }
    }

    return returnEpc;
};
