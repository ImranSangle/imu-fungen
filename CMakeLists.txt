
cmake_minimum_required(VERSION 3.31)

project(imufungen)

enable_testing()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions("-DDEBUG")
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(imufungen STATIC src/imufungen.cpp src/mathfunctions.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE imufungen)

add_subdirectory(test)
if(EXAMPLE)
add_subdirectory(examples)
endif()
