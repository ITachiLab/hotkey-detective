cmake_minimum_required(VERSION 3.17)
project(Common)

add_library(${PROJECT_NAME} INTERFACE)
add_library(HkdCommon ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE
        include)
