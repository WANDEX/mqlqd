## src

add_library(mqlqd_bin_i INTERFACE)

target_include_directories(mqlqd_bin_i INTERFACE cxxopts)
target_link_libraries(mqlqd_bin_i INTERFACE cxxopts)

target_link_libraries(mqlqd_bin_i INTERFACE wandex::mqlqd::dev)


add_subdirectory(client)
add_subdirectory(daemon)

