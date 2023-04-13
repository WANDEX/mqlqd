## src

get_filename_component(common_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/common REALPATH)

## common sources for the both targets: client & daemon
set(common_src
  ${common_src_dir}/file.cpp
)

add_library(mqlqd_bin_i INTERFACE)

target_include_directories(mqlqd_bin_i INTERFACE cxxopts)
target_link_libraries(mqlqd_bin_i INTERFACE cxxopts)

target_link_libraries(mqlqd_bin_i INTERFACE wandex::mqlqd::dev)


add_subdirectory(client)
add_subdirectory(daemon)

