cmake_minimum_required(VERSION 3.10)
project(RISCVOS)

enable_language(C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER toolchain/riscv/bin/riscv64-unknown-linux-gnu-gcc)
set(CMAKE_LINKER toolchain/riscv/bin/riscv64-unknown-linux-gnu-ld)
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/src/linker.ld)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/dist/riscv)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(TARGET kernel)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "-Wno-int-to-pointer-cast -fpermissive -Wno-conversion -Wno-narrowing -Wno-everything -fno-exceptions --target=riscv64 -mabi=lp64d -march=rv64gc -mcmodel=medany -g -O0 -c -Wall -Wextra")
set(CMAKE_ASM_FLAGS "-Wno-int-to-pointer-cast -fpermissive -Wno-conversion -Wno-narrowing -Wno-everything -mabi=lp64d -march=rv64gc -mcmodel=medany -g -O0 -c -Wall -Wextra")

add_compile_definitions(ARCH=RISCV64)

add_subdirectory(src)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} -n -g -T ${LINKER_SCRIPT} <LINK_LIBRARIES> <OBJECTS> -lgcc -Ltoolchain/riscv/lib/gcc/riscv64-unknown-linux-gnu/13.2.0/ -lsupc++ -Ltoolchain/riscv/riscv64-unknown-linux-gnu/lib/ -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.elf ")

add_executable(${TARGET}.elf $<TARGET_OBJECTS:Kernel> $<TARGET_OBJECTS:Utils>)

function(get_all_sources target destination)
    get_target_property(sources ${target} CXX_SOURCES)
    set(${destination} ${sources} PARENT_SCOPE)
endfunction()

get_all_sources(Kernel KernelSources)
get_all_sources(Utils UtilsSources)

add_custom_target(
        format
        COMMAND clang-format -i -style=file ${KernelSources} ${UtilsSources}
)
