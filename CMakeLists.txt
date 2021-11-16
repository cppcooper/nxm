cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-fmodules-ts)

# project variables
project(nxm)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# dependencies
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false src/*)
file(GLOB_RECURSE QGP_SOURCES false extern/gumbo-wrapper/src/*)
set(CPR_DIR .) #cpr-config.cmake directory
find_package(CPR)
include_directories(include)
include_directories(extern/gumbo-wrapper/src)
include_directories(extern/nlohmann-json/single_include)
include_directories(extern/cli11/include)
link_directories(extern/gumbo-query/lib)

# target
set(EXE_TARGET ${CMAKE_PROJECT_NAME})
add_executable(${EXE_TARGET} ${SOURCES} ${QGP_SOURCES})
target_link_libraries(${EXE_TARGET} cpr libgumbo.so)

# install target
install(TARGETS ${EXE_TARGET} DESTINATION bin)