cmake_minimum_required(VERSION 3.17)
project(hotkey_detective C)

set(CMAKE_C_STANDARD 99)

add_executable(hotkey_detective src/main.c src/core.c res/resources.rc)
add_library(hotkey_hook SHARED src/dllmain.c)

target_include_directories(hotkey_detective PUBLIC include)
target_include_directories(hotkey_hook PUBLIC include)