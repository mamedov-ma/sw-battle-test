cmake_minimum_required(VERSION 3.22.1)
project(sw_battle_test)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp "${CMAKE_CURRENT_LIST_DIR}/src/sw/units.cpp")
add_executable(sw_battle_test ${SOURCES})

target_include_directories(sw_battle_test PUBLIC src/)
