## common sources to the both targets: client & daemon

target_sources(mqlqd_src
  PRIVATE
    sig_handler.cpp
    file.cpp
    log.cpp
    op.cpp
    fclient.cpp
    fserver.cpp
)
