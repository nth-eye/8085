cmake_minimum_required(VERSION 3.0.0)
project(8085 VERSION 0.1.0)

file(GLOB SRC_FILES src/*.cpp)

add_executable(8085 main.cpp ${SRC_FILES})
target_link_libraries(8085)
include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
