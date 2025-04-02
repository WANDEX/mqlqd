## common sources to the both targets: client & daemon

target_sources(wndx_sane_src
  PRIVATE
    sig_handler.cpp
    file.cpp
    log.cpp
)

## XXX: only to make wndx_sane_deps target archive not empty =>
## AppleClang will not compile static library without objects in archive.
target_sources(wndx_sane_deps PRIVATE log.cpp)

