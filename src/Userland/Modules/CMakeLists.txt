# src/Userland/Modules/CMakeLists.txt

# 1. Define where the Module Linker Script is
set(MODULE_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/src/Userland/Modules/module.ld")

# 2. Allow user to select modules (Default: ALL)
set(MATTEOS_BUILD_MODULES "ALL" CACHE STRING "Semicolon-separated list of modules to build, or 'ALL'")

# =========================================================
# Helper Function: add_matteos_module
# Usage: add_matteos_module(Name Sources...)
# =========================================================
function(add_matteos_module TARGET_NAME)
    # --- Check if we should build this module ---
    if(NOT "${MATTEOS_BUILD_MODULES}" STREQUAL "ALL" AND NOT ";${MATTEOS_BUILD_MODULES};" MATCHES ";${TARGET_NAME};")
        return()
    endif()

    message(STATUS "Configuring module: ${TARGET_NAME}")

    # =========================================================
    # STEP 1: Compile Sources to Objects (No Linking)
    # =========================================================
    # We use an OBJECT library. This compiles .cpp to .o but does NOT invoke ld.
    add_library(${TARGET_NAME}_objs OBJECT ${ARGN})

    target_include_directories(${TARGET_NAME}_objs PRIVATE
            ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_SOURCE_DIR}/src/Userland/Libraries
    )

    target_compile_options(${TARGET_NAME}_objs PRIVATE
            -fPIC
            -mabi=lp64d
            -march=rv64gc
            -fno-builtin
            -nostdlib
            -fno-exceptions
            -fno-rtti
            -mcmodel=medany
    )

    # =========================================================
    # STEP 2: Manually Link Objects to .ko
    # =========================================================
    set(MODULE_OUTPUT_PATH "${DIST_DIR}/modules/${TARGET_NAME}.ko")

    add_custom_command(
            OUTPUT ${MODULE_OUTPUT_PATH}
            # We call the linker directly (CMAKE_LINKER is usually ld)
            # -r: Generate relocatable output (Partial Linking)
            # -T: Use your script to order sections (.module_header first)
            COMMAND ${CMAKE_LINKER} -r -T "${MODULE_LINKER_SCRIPT}" -o ${MODULE_OUTPUT_PATH} $<TARGET_OBJECTS:${TARGET_NAME}_objs>

            DEPENDS ${TARGET_NAME}_objs "${MODULE_LINKER_SCRIPT}"
            COMMENT "Linking Kernel Module: ${TARGET_NAME}.ko"
    )

    # =========================================================
    # STEP 3: Create a Target to drive the build
    # =========================================================
    add_custom_target(${TARGET_NAME} ALL DEPENDS ${MODULE_OUTPUT_PATH})
    add_dependencies(modules ${TARGET_NAME})

endfunction()

# =========================================================
# Main Target "modules"
# =========================================================
add_custom_target(modules)

# Automatic Discovery
# Looks for CMakeLists.txt in subdirectories
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
        add_subdirectory(${child})
    endif()
endforeach()

