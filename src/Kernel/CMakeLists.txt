set(ASM_SOURCE  
            Arch/riscv/boot.S
            Arch/riscv/mem.S
            Arch/riscv/trap.S
)
set(SOURCE
            Init.cpp
            CPU.cpp
            Memory/MemoryManager.cpp
            Memory/KernelMemoryAllocator.cpp
            Memory/MemoryRegion.cpp
            Memory/Page.cpp
            Memory/PageTable.cpp
            Memory/PageTableEntry.cpp
            Uart.cpp
            System/Trap.cpp
            System/Plic.cpp
            System/System.cpp
            Process/Process.cpp
            Process/ProcessManager.cpp
            Process/Scheduler.cpp
        Sbi/sbi.cpp
)

include_directories(${SOURCE_DIRECTORY})
add_library(Kernel OBJECT ${SOURCE} ${ASM_SOURCE})
#set_target_properties( PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")