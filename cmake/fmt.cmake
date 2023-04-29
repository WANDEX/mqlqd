## prefer fmtlib version installed in the system || fetch if not found


# find_package(fmt 9.1.0)
# if(NOT fmt_FOUND)
if(TRUE)
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

get_filename_component(fmt_src
  ${PROJECT_BINARY_DIR}/_deps/fmt-src REALPATH
)
# target_include_directories(mqlqd_deps PUBLIC ${fmt_src}/include)
target_include_directories(mqlqd_deps PRIVATE ${fmt_src}/include)
# target_precompile_headers(mqlqd_deps PRIVATE ${fmt_src}/cxxopts.hpp)

# add_subdirectory(${fmt_src})

