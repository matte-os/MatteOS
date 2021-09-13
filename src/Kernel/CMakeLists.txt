set(ASM_SOURCE  
            Arch/riscv/boot.S
            Arch/riscv/mem.S
            Arch/riscv/trap.S
)
set(SOURCE  
            Init.cpp
            CPU.cpp
            Page.cpp
            PageController.cpp
            PageTable.cpp
            PageTableEntry.cpp
            Uart.cpp
            Trap.cpp
)

include_directories(${SOURCE_DIRECTORY})
add_library(Kernel OBJECT ${SOURCE} ${ASM_SOURCE})
#set_target_properties( PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")