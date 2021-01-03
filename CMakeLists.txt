cmake_minimum_required(VERSION 3.17)
project(hotkey_detective)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DUNICODE)

add_executable(hotkey_detective WIN32
        src/HotkeyTable.cpp
        src/main.cpp
        src/core.cpp
        res/resources.rc)

add_library(hotkey_hook SHARED src/dllmain.c)

target_link_libraries(hotkey_detective hotkey_hook comctl32)

target_include_directories(hotkey_detective PUBLIC include res)
target_include_directories(hotkey_hook PUBLIC include)