## prefer fmtlib version installed in the system || fetch if not found
find_package(fmt 9.1.0)
# find_package(fmt)
if(NOT fmt_FOUND)
  message(">> fetching fmt of required version!")
  include(FetchContent)
  FetchContent_Declare(
    # fmtlib # release 9.1.0
    # fmt # release 9.1.0
    libfmt # release 9.1.0
    URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip
  )
  # FetchContent_MakeAvailable(fmtlib)
  # FetchContent_MakeAvailable(fmt)
  FetchContent_MakeAvailable(libfmt)
else()
  message(">> found fmt of required version!")
endif()
# include(fmt)
# add_subdirectory(fmt)

