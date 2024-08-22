cmake_minimum_required(VERSION 3.17)
project(Hook)

add_definitions(-DDLL_COMPILE)

add_library(${PROJECT_NAME} SHARED
        src/HkdHook.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "hook")

target_link_libraries(${PROJECT_NAME} Shlwapi)
target_include_directories(${PROJECT_NAME} PUBLIC include)