set(SOURCE  Assertions.cpp
            Basic.cpp
            DebugConsole.cpp
            Optional.cpp
            cxx.cpp
            Memory.cpp
            Strings/StringValue.cpp
            Strings/String.cpp
            kmalloc.cpp
    )
include_directories(${SOURCE_DIRECTORY})
add_library(Utils OBJECT ${SOURCE})

set(CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)

set_property(TARGET Utils PROPERTY CXX_SOURCES ${SOURCE})