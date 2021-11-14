cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

# project variables
project(nxm)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# dependencies
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false src/*)
add_subdirectory(extern/gumbo-query)
add_subdirectory(extern/cpr)
add_subdirectory(extern/cli11)
add_subdirectory(extern/nlohmann-json)
include_directories(include)
include_directories(extern/gumbo-query/include)
include_directories(extern/nlohmann-json/single_include)
include_directories(extern/cpr/include)
include_directories(extern/cli11/include)

# target
set(EXE_TARGET ${CMAKE_PROJECT_NAME})
add_executable(${EXE_TARGET} ${SOURCES})
