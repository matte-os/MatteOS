set(SOURCE  Assertions.cpp
            Basic.cpp
            DebugConsole.cpp
            Optional.cpp
            cxx.cpp
            Memory.cpp
            Arrays/ArrayList.cpp
            Strings/StringImpl.cpp
            Strings/String.cpp
            kmalloc.cpp
    )
include_directories(${SOURCE_DIRECTORY})
add_library(Utils OBJECT ${SOURCE})