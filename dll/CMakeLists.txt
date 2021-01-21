cmake_minimum_required(VERSION 3.17)
project(hotkey_hook)

add_definitions(-DDLL_COMPILE)

set(CMAKE_C_STANDARD C99)

add_library(hotkey_hook SHARED dllmain.c)