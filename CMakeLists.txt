cmake_minimum_required(VERSION 3.10)
project(RISCVOS)

enable_language(C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang++)
set(CMAKE_LINKER riscv64-unknown-linux-gnu-ld)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/src/linker.ld)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/riscv)
set(TARGET kernel)


add_compile_options(-Wno-int-to-pointer-cast -fpermissive -Wno-conversion -Wno-narrowing -Wno-everything --target=riscv64 -mabi=lp64 -march=rv64gc -mcmodel=medany -c)

#include_directories(${CMAKE_SOURCE_DIR}/lib/opensbi/install/usr/include)
add_subdirectory(src)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} -n -g -T ${LINKER_SCRIPT} <LINK_LIBRARIES> <OBJECTS> -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.elf ")
#set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} -n -T ${LINKER_SCRIPT} <LINK_LIBRARIES> <OBJECTS> -lplatsbi -Llib/opensbi/install/usr/lib64/lp64/opensbi/generic/lib/ -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.elf ")


add_executable(${TARGET}.elf $<TARGET_OBJECTS:Kernel> $<TARGET_OBJECTS:Utils>)
#target_link_libraries(${TARGET}.elf ${CMAKE_SOURCE_DIR}/lib/opensbi/install/usr/lib64/lp64/libsbi.a ${CMAKE_SOURCE_DIR}/lib/opensbi/install/usr/lib64/lp64/libsbiutils.a)
