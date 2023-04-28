## src

get_filename_component(common_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/common REALPATH)

## common sources for the both targets: client & daemon
set(common_src
  ${common_src_dir}/sig_handler.cpp
  ${common_src_dir}/file.cpp
  ${common_src_dir}/log.cpp
  ${common_src_dir}/op.cpp
  ${common_src_dir}/fclient.cpp
  ${common_src_dir}/fserver.cpp
)

add_library(mqlqd_bin_i INTERFACE)

target_link_libraries(mqlqd_bin_i INTERFACE wandex::mqlqd::dev)

## link with the libc -lc (to work with the Linux socket API interface)
target_link_libraries(mqlqd_bin_i INTERFACE c)


add_subdirectory(client)
add_subdirectory(daemon)

