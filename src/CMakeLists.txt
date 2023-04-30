## src

if(TARGET mqlqd_deps)
  target_link_libraries(mqlqd_src PUBLIC mqlqd_deps)
endif()

if(TARGET mqlqd_bin_i)
  # target_link_libraries(mqlqd_daemon PUBLIC mqlqd_bin_i)
  # target_link_libraries(mqlqd_daemon INTERFACE mqlqd_bin_i)
endif()

add_subdirectory(common)
# add_subdirectory(client)
add_subdirectory(daemon)

