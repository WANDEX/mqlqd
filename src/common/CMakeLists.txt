## common sources to the both targets: client & daemon

target_sources(wndx_sane_src
  PRIVATE
    sig_handler.cpp
    file.cpp
    log.cpp
)
