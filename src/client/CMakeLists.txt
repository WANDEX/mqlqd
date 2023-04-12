## client

file(GLOB_RECURSE client_sources LIST_DIRECTORIES false *.hpp *.cpp)

add_executable(mqlqd_client ${client_sources})


if(TARGET mqlqd_bin_i)
  target_link_libraries(mqlqd_client PUBLIC mqlqd_bin_i)
endif()

