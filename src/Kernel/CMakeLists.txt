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
        System/DeviceManager.cpp
        System/DriverManager.cpp
        System/Trap.cpp
        System/Plic.cpp
        System/System.cpp
        System/SyscallManager.cpp
        System/Timer.cpp
        Process/Process.cpp
        Process/ProcessManager.cpp
        Process/Scheduler.cpp
        Sbi/sbi.cpp
        FileSystem/VirtualFileSystem.cpp
        FileSystem/FileSystem.cpp
        FileSystem/Inode.cpp
        FileSystem/File.cpp
        Firmware/FDT/FDTParser.cpp
        VirtIO/VirtIODeviceIDs.cpp
        VirtIO/VirtQueue.cpp
)

include_directories(${SOURCE_DIRECTORY})
add_library(Kernel OBJECT ${SOURCE} ${ASM_SOURCE})

set(CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

list(TRANSFORM ASM_SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)
list(TRANSFORM SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)

set_property(TARGET Kernel PROPERTY ALL_SOURCES ${SOURCE} ${ASM_SOURCE})
set_property(TARGET Kernel PROPERTY ASM_SOURCES ${ASM_SOURCE})
set_property(TARGET Kernel PROPERTY CXX_SOURCES ${SOURCE})
#set_target_properties( PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")