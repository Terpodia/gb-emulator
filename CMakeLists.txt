cmake_minimum_required(VERSION 3.8)
project(gb_emulator)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)

add_executable(gbemu main.cpp)

target_link_libraries(gbemu emu)
