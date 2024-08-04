cmake_minimum_required(VERSION 3.17)
project(hotkey_detective)

add_subdirectory(dll)

set(CMAKE_CXX_STANDARD 17)

# This option will terminate all strings in the resources with proper null
# characters.
set(CMAKE_RC_FLAGS "/n")

add_definitions(-DUNICODE)

add_executable(HotkeyDetective WIN32
        src/MainWindow.cpp
        src/HotkeyTable.cpp
        src/WindowsUtils.cpp
        src/Core.cpp
        src/KeySequence.cpp
        src/main.cpp
        res/project/project.rc)

target_link_libraries(HotkeyDetective
        hotkey_hook
        comctl32)

target_include_directories(HotkeyDetective PUBLIC
        include
        res/project)