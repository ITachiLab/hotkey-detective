cmake_minimum_required(VERSION 3.17)
project(Hook)
include(CMakePrintHelpers)
cmake_print_variables(PROJECT_BINARY_DIR)
add_definitions(-DDLL_COMPILE)

add_library(${PROJECT_NAME} SHARED
        src/HkdHook.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "hook")
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE)

target_link_libraries(${PROJECT_NAME} HkdCommon Shlwapi)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_custom_command(TARGET Hook POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Hook> ${PROJECT_BINARY_DIR}/../detective
)