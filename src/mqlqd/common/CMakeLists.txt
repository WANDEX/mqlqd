## common sources to the both targets: client & daemon

target_sources(mqlqd_src
  PRIVATE
    sig_handler.cpp
    file.cpp
)

## XXX: only to make target archive not empty =>
## Xcode AppleClang will not compile static library without objects in archive.
target_sources(mqlqd_deps PRIVATE file.cpp)

