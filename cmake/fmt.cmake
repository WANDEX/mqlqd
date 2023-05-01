## prefer fmtlib version installed in the system || fetch if not found

cmake_path(APPEND dep_dir ${PROJECT_BINARY_DIR} "_deps")

cmake_path(APPEND fmt_dir ${dep_dir} "fmt") # lib dir root
cmake_path(APPEND fmt_sub ${fmt_dir} "sub")
cmake_path(APPEND fmt_src ${fmt_dir} "src")
cmake_path(APPEND fmt_bin ${fmt_dir} "bin")

# find_package(fmt 9.1.0)
# if(NOT fmt_FOUND)
if(TRUE)
  message(">> fetching fmt of required version!")
  include(FetchContent)
  FetchContent_Declare(fmt
    GIT_REPOSITORY    https://github.com/fmtlib/fmt.git
    GIT_TAG           9.1.0
    SUBBUILD_DIR      ${fmt_sub}
    SOURCE_DIR        ${fmt_src}
    BINARY_DIR        ${fmt_bin}
  )
  option(FMT_INSTALL "" ON)
  FetchContent_MakeAvailable(fmt)
else()
  message(">> found fmt of required version!")
endif()

target_include_directories(mqlqd_deps PUBLIC ${fmt_src}/include)
# target_include_directories(mqlqd_deps INTERFACE ${fmt_src}/include)

## NOTE: this will work only for linking with the system installed libfmt
# target_link_libraries(mqlqd_deps -lfmt)

## link with the static library libfmtd.a
## which is found in the fetched locally lib dir.
# target_link_libraries(mqlqd_deps libfmtd.a)
target_link_libraries(mqlqd_deps -lfmtd) # i.e. libfmtd.a
target_link_libraries(mqlqd_deps -L${fmt_bin})


## XXX: THIS does not work!
## CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
## Please set them or make sure they are set and tested correctly in the CMake files:
## FMT_LIB
# find_library(FMT_LIB fmt)
# This call requires CMake 3.13 or later
# target_link_libraries(mqlqd_deps PUBLIC ${FMT_LIB})


# target_link_libraries(mqlqd_deps PRIVATE fmt::fmt)
# target_link_libraries(mqlqd_deps PUBLIC fmt::fmt)

# target_link_libraries(mqlqd_depsi INTERFACE fmt::fmt-header-only)
# target_link_libraries(mqlqd_deps PUBLIC fmt::fmt-header-only)

