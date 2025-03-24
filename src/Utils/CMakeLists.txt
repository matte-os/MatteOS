set(SOURCE Assertions.cpp
        Basic.cpp
        DebugConsole.cpp
        cxx.cpp
        Memory.cpp
        Strings/StringValue.cpp
        Strings/String.cpp
        Strings/StringBuilder.cpp
        Hashing/HashFunctions.cpp
        kmalloc.cpp
        Types.cpp
        Strings/Formatter.cpp
        Math.cpp
)
include_directories(SYSTEM ${SOURCE_DIRECTORY})
add_library(Utils OBJECT ${SOURCE})

set(CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(TRANSFORM SOURCE PREPEND ${CURRENT_SOURCE_DIR}/)

set_property(TARGET Utils PROPERTY CXX_SOURCES ${SOURCE})