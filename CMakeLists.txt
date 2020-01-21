cmake_minimum_required(VERSION 3.6)
project(MWStatusBar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0")

set(SOURCE_FILES main.cpp)
add_executable(MWStatusBar ${SOURCE_FILES})
target_link_libraries(MWStatusBar -pthread sfml-graphics sfml-window
        sfml-system)