file (GLOB headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

file (GLOB sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/lib/*.cpp")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_library(emu STATIC ${headers} ${sources})

target_include_directories(emu PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(emu ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
