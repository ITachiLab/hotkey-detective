cmake_minimum_required(VERSION 3.17)
project(hotkey_detective)

add_subdirectory(dll)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DUNICODE)

add_executable(HotkeyDetective WIN32
        src/MainWindow.cpp
        src/HotkeyTable.cpp
        src/Core.cpp
        src/main.cpp
        res/resources.rc)

target_link_libraries(HotkeyDetective hotkey_hook comctl32)

target_include_directories(HotkeyDetective PUBLIC include res)