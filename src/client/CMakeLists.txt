## client

file(GLOB_RECURSE client_sources LIST_DIRECTORIES false *.hpp *.cpp)

add_executable(mqlqd_client ${common_src} ${client_sources})


if(TARGET mqlqd_bin_i)
  target_link_libraries(mqlqd_client PUBLIC mqlqd_bin_i)
endif()

if(TARGET mqlqd_deps)
  target_sources(mqlqd_deps PRIVATE client.cpp)
  target_link_libraries(mqlqd_client PRIVATE mqlqd_deps)
endif()

