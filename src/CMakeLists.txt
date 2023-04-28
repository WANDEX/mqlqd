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

add_subdirectory(client)
add_subdirectory(daemon)

