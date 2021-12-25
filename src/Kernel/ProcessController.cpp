#include <Kernel/ProcessController.hh>
#include <Kernel/PageController.hh>

namespace Kernel
{
    ProcessController::ProcessController()
    {
        nextPid = 1;
    }

    ProcessController::~ProcessController()
    {
    }

    Process ProcessController::initProcess(uintptr_t function){
        Process process(nextPid++, 
                        ProcessStatus::WAITING, 
                        TrapFrame(), 
                        (PageTable*)PageController::alloc(1), 
                        STARTING_ADDRESS, 
                        (u64*)PageController::alloc(STACK_PAGES)
                    );
        
        auto& frame = process.getFrame();
        //Setting the virtual address of the stack to the stack pointer register.
        frame.regs[2] = STACK_ADDRESS + PageController::PAGE_SIZE * STACK_PAGES;
        
        auto& table = process.getPageTable();

        //Mapping the stack
        for(int i = 0; i < STACK_PAGES; i++){
            size_t addr = i * PageController::PAGE_SIZE;
            PageController::map(
                table,
                (VirtualAddress) (STACK_ADDRESS + addr),
                (PhysicalAddress) ((uintptr_t)process.getStack() + addr),
                (size_t) EntryBits::USER_READ_WRITE,
                0
            );
        }

        //Mapping the program counter
        PageController::map(
            table,
            (VirtualAddress) STARTING_ADDRESS,
            (PhysicalAddress) function,
            (size_t) EntryBits::USER_READ_EXECUTE,
            0
        );

        PageController::map(
            table,
            (VirtualAddress) (STARTING_ADDRESS + 0x1001),
            (PhysicalAddress) (function + 0x1001),
            (size_t) EntryBits::USER_READ_EXECUTE,
            0
        );
        return process;
    }
};