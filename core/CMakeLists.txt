file (GLOB_RECURSE core_headers CONFIGURE_DEPENDS "include/*.h")

file (GLOB_RECURSE core_sources CONFIGURE_DEPENDS "lib/*.cpp")

add_library(gbcore STATIC ${core_headers} ${core_sources})

target_include_directories(gbcore PUBLIC include)

