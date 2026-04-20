find_package(SDL3 REQUIRED)

add_executable(gbemu main.cpp desktop_ui.cpp desktop_audio.cpp desktop_read_cart.cpp desktop_save_data.cpp)

target_link_libraries(gbemu gbcore SDL3::SDL3)

