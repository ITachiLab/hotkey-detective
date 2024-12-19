cmake_minimum_required(VERSION 3.17)
project(Tester)

add_executable(${PROJECT_NAME} WIN32
        res/project/project.rc
        src/main.cpp
        src/TesterWindow.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE)
target_include_directories(${PROJECT_NAME} PUBLIC
        include
        res/project)

target_link_libraries(${PROJECT_NAME} PUBLIC HkdCommon)
