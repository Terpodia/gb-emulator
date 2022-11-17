cmake_minimum_required(VERSION 3.8)
project(gb_emulator)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)

add_executable(gbemu main.cpp)

target_link_libraries(gbemu emu)
