# src

add_executable(mqlqd_client ./client/client.cpp)

target_include_directories(mqlqd_client PUBLIC cxxopts)
target_link_libraries(mqlqd_client PUBLIC cxxopts)

target_link_libraries(mqlqd_client PUBLIC wandex::mqlqd::dev)

