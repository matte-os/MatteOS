cmake_minimum_required(VERSION 3.20)
project(MatteOS)

# ==========================================
# 1. Toolchain & Environment Setup
# ==========================================
enable_language(C CXX ASM)

# Toolchain Paths
set(TOOLCHAIN_PATH "${PROJECT_SOURCE_DIR}/toolchain/riscv/bin")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/riscv64-unknown-linux-gnu-gcc")
set(CMAKE_LINKER "${TOOLCHAIN_PATH}/riscv64-unknown-linux-gnu-ld")

# Project Paths
set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/src/linker.ld")
set(DIST_DIR "${PROJECT_SOURCE_DIR}/build/dist/riscv")
set(TOOLS_DIR "${PROJECT_SOURCE_DIR}/tools")
set(SCRIPT_DIR "${PROJECT_SOURCE_DIR}/scripts")

# Output Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# ==========================================
# 2. Compiler Flags & Definitions
# ==========================================
# Global definitions
add_compile_definitions(ARCH=RISCV64)

set(COMMON_FLAGS
        -mabi=lp64d
        -march=rv64gc
        -mcmodel=medany
        -g -O0
        -c
        -Wall -Wextra
        -Wno-int-to-pointer-cast
        -Wno-conversion
        -Wno-unused-parameter
)

add_compile_options(${COMMON_FLAGS})

# --- B. C++ Specific Flags (Clang Only) ---
# We add '--target' here because only Clang needs it.
# We use separate calls to avoid list expansion bugs in generator expressions.

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:--target=riscv64>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>)

# --- C. ASM Specific Flags (GCC Only) ---
# GCC might need to know it's preprocessing assembly manually if files end in .S
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-xassembler-with-cpp>)

# ==========================================
# 3. Custom Linker Command
# ==========================================
# We override the link rule to use LD directly with our script.
# Note: Changed hardcoded path to <TARGET> so it works for both passes.
set(CMAKE_CXX_LINK_EXECUTABLE
        "${CMAKE_LINKER} -n -g -T ${LINKER_SCRIPT} <OBJECTS> -o <TARGET>"
)

include_directories(${PROJECT_SOURCE_DIR}/src)

# ==========================================
# 4. Build Targets (Two-Pass Logic)
# ==========================================
add_subdirectory(src)

# --- Pass 1: Temporary Kernel with Dummy Symbols ---
# We create a dummy file so the linker doesn't complain about missing symbols
set(DUMMY_SYMS_FILE "${DIST_DIR}/generated/DummyKernelSymbols.cpp")
if (NOT EXISTS ${DUMMY_SYMS_FILE})
    file(WRITE ${DUMMY_SYMS_FILE}
            "#include <Utils/Types.h> \n extern \"C\" { void* k_exported_symbols[] = { 0 }; const size_t k_exported_symbols_count = 0; }"
    )
endif ()

# Define the temporary kernel
add_executable(kernel_temp.elf
        ${DUMMY_SYMS_FILE}
        $<TARGET_OBJECTS:Kernel>
        $<TARGET_OBJECTS:Utils>
)

# --- Pass 2: Generate Real Symbols ---
set(GEN_SCRIPT "${SCRIPT_DIR}/generate_kernel_symbols.py")
set(ALLOWED_SYMS_LIST "${PROJECT_SOURCE_DIR}/scripts/allowed_symbols.txt")
set(REAL_SYMS_FILE "${DIST_DIR}/generated/GeneratedKernelSymbols.cpp")

# Run the python script on kernel_temp.elf to create generated_ksyms.cpp
add_custom_command(
        OUTPUT ${REAL_SYMS_FILE}
        COMMAND python3 ${GEN_SCRIPT}
        ${DIST_DIR}/kernel_temp.elf
        --allowed-list ${ALLOWED_SYMS_LIST}
        --nm-tool ${CMAKE_NM} # CMake usually auto-detects this
        > ${REAL_SYMS_FILE}
        DEPENDS kernel_temp.elf ${GEN_SCRIPT} ${ALLOWED_SYMS_LIST}
        COMMENT "Filtering and generating kernel symbols..."
)

# --- Pass 3: Final Kernel ---
add_executable(kernel.elf
        ${REAL_SYMS_FILE}
        $<TARGET_OBJECTS:Kernel>
        $<TARGET_OBJECTS:Utils>
)
# Ensure we always rebuild if temp changes
add_dependencies(kernel.elf kernel_temp.elf)

# ==========================================
# 5. Utilities (Formatting)
# ==========================================
function(get_all_sources target destination)
    get_target_property(sources ${target} CXX_SOURCES)
    set(${destination} ${sources} PARENT_SCOPE)
endfunction()

get_all_sources(Kernel KernelSources)
get_all_sources(Utils UtilsSources)

add_custom_target(format
        COMMAND clang-format -i -style=file ${KernelSources} ${UtilsSources}
        COMMENT "Formatting sources..."
)