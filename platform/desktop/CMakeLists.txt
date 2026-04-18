find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

add_executable(gbemu main.cpp desktop_ui.cpp desktop_audio.cpp desktop_read_cart.cpp desktop_save_data.cpp)
target_link_libraries(gbemu gbcore ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
target_include_directories(gbemu PRIVATE ${SDL2_INCLUDE_DIRS})

