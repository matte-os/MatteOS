set(SOURCE  Assertions.cpp
            Basic.cpp
            DebugConsole.cpp
            Option.cpp
    )
include_directories(${SOURCE_DIRECTORY})
add_library(Utils OBJECT ${SOURCE})