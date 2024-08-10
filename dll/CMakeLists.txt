cmake_minimum_required(VERSION 3.17)
project(hotkey_hook)

add_definitions(-DDLL_COMPILE -DUNICODE)

set(CMAKE_CXX_STANDARD 17)

add_library(hotkey_hook SHARED HkdHook.cpp)
target_link_libraries(hotkey_hook Shlwapi)