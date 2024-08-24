set(ASM_SOURCE
        Arch/riscv64/boot.S
        Arch/riscv64/mem.S
        Arch/riscv64/trap.S
)
set(SOURCE
        Init.cpp
        Arch/riscv64/CPU.cpp
        Arch/riscv64/Interrupts/Interrupts.cpp
        FileSystem/DEVFS/DevFileSystem.cpp
        FileSystem/DEVFS/DevInode.cpp
        FileSystem/RAMFS/RamFileSystem.cpp
        FileSystem/RAMFS/RamInode.cpp
        FileSystem/VirtualFileSystem.cpp
        FileSystem/FileSystem.cpp
        FileSystem/Inode.cpp
        FileSystem/FileDescriptor.cpp
        FileSystem/FileDescriptorTable.cpp
        FileSystem/File.cpp
        Firmware/FDT/FDTParser.cpp
        Firmware/DeviceTree.cpp
        Memory/MemoryManager.cpp
        Memory/KernelMemoryAllocator.cpp
        Memory/MemoryRegion.cpp
        Memory/Page.cpp
        Memory/PageTable.cpp
        Memory/PageTableEntry.cpp
        Uart.cpp
        Syscalls/SyscallManager.cpp
        Syscalls/SyscallOpen.cpp
        Devices/DeviceManager.cpp
        Drivers/DriverManager.cpp
        System/InterruptManager.cpp
        Arch/riscv64/Interrupts/Plic.cpp
        System/System.cpp
        System/Timer.cpp
        Process/Process.cpp
        Process/ProcessManager.cpp
        Process/Scheduler.cpp
        SBI/SBI.cpp
        Drivers/VirtIO/VirtIODeviceIDs.cpp
        Drivers/VirtIO/VirtQueue.cpp
        Drivers/VirtIO/BlockIO.cpp
        Drivers/VirtIO/BlockIODriver.cpp
        Drivers/Driver.cpp
        Devices/Device.cpp
)

include_directories(SYSTEM ${SOURCE_DIRECTORY})
add_library(Kernel OBJECT ${SOURCE} ${ASM_SOURCE})

set(CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

list(TRANSFORM ASM_SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)
list(TRANSFORM SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)

set_property(TARGET Kernel PROPERTY ALL_SOURCES ${SOURCE} ${ASM_SOURCE})
set_property(TARGET Kernel PROPERTY ASM_SOURCES ${ASM_SOURCE})
set_property(TARGET Kernel PROPERTY CXX_SOURCES ${SOURCE})
#set_target_properties( PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")