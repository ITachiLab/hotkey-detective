cmake_minimum_required(VERSION 3.17)
project(hotkey_detective)

set(CMAKE_CXX_STANDARD 17)

# This option will terminate all strings in the resources with proper null
# characters.
set(CMAKE_RC_FLAGS "/n")

add_compile_definitions(-DUNICODE)

add_subdirectory(common)
add_subdirectory(detective)
add_subdirectory(hook)