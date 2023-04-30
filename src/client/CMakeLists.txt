## client

add_executable(mqlqd_client "")

target_sources(mqlqd_client
  PRIVATE
    cmd.cpp
    client.cpp
)

target_link_libraries(mqlqd_client PRIVATE mqlqd_src)

