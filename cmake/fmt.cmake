## prefer fmtlib version installed in the system || fetch if not found
find_package(fmt 9.1.0)
if(NOT fmt_FOUND)
  message(">> fetching fmt of required version!")
  include(FetchContent)
  FetchContent_Declare(
    fmt # release 9.1.0
    URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip
  )
  FetchContent_MakeAvailable(fmt)
else()
  message(">> found fmt of required version!")
endif()

