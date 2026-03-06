## common sources to the both targets: client & daemon

target_sources(mqlqd_src
  PRIVATE
    file.cpp
    unix_sig.cpp
)

