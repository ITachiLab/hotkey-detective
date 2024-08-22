cmake_minimum_required(VERSION 3.17)
project(Detective)

add_executable(${PROJECT_NAME} WIN32
        src/MainWindow.cpp
        src/HotkeyTable.cpp
        src/WindowsUtils.cpp
        src/Core.cpp
        src/KeySequence.cpp
        src/main.cpp
        res/project/project.rc)

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "Hotkey Detective")

target_link_libraries(${PROJECT_NAME} PUBLIC
        HkdCommon
        Hook
        comctl32)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        res/project)