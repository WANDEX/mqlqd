## daemon

file(GLOB_RECURSE daemon_sources LIST_DIRECTORIES false *.hpp *.cpp)

add_executable(mqlqd_daemon ${common_src} ${daemon_sources})


if(TARGET mqlqd_bin_i)
  target_link_libraries(mqlqd_daemon PUBLIC mqlqd_bin_i)
endif()

if(TARGET mqlqd_deps)
  target_sources(mqlqd_deps PRIVATE daemon.cpp)
  target_link_libraries(mqlqd_daemon PRIVATE mqlqd_deps)
endif()

