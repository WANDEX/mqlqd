## daemon

add_executable(mqlqd_daemon "")

target_sources(mqlqd_daemon
  PRIVATE
    cmd.cpp
    daemon.cpp
)

target_link_libraries(mqlqd_daemon PRIVATE mqlqd_src)

