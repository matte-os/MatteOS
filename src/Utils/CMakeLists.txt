set(SOURCE  Assertions.cpp
            Basic.cpp
            DebugConsole.cpp
            Optional.cpp
            cxx.cpp
            Memory.cpp
    )
include_directories(${SOURCE_DIRECTORY})
add_library(Utils OBJECT ${SOURCE})