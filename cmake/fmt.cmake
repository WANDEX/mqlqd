## prefer fmtlib version installed in the system || fetch if not found

cmake_path(APPEND dep_dir ${PROJECT_BINARY_DIR} "_deps")

cmake_path(APPEND fmt_dir ${dep_dir} "fmt") # lib dir root
cmake_path(APPEND fmt_src ${fmt_dir} "src")
cmake_path(APPEND fmt_bld ${fmt_dir} "build")
cmake_path(APPEND fmt_sub ${fmt_dir} "subbuild")

# find_package(fmt 9.1.0)
# if(NOT fmt_FOUND)
if(TRUE)
  message(">> fetching fmt of required version!")
  include(FetchContent)
  FetchContent_Declare(fmt
    GIT_REPOSITORY    https://github.com/fmtlib/fmt.git
    GIT_TAG           9.1.0
    SOURCE_DIR        ${fmt_src}
    SUBBUILD_DIR      ${fmt_bld}
    BINARY_DIR        ${fmt_sub}
  )
  FetchContent_MakeAvailable(fmt)
else()
  message(">> found fmt of required version!")
endif()

# get_filename_component(fmt_src ${PROJECT_BINARY_DIR}/_deps/fmt-src REALPATH)

# target_include_directories(mqlqd_deps PUBLIC ${fmt_src}/include)
# target_include_directories(mqlqd_deps PRIVATE ${fmt_src}/include)
# target_precompile_headers(mqlqd_deps PRIVATE ${fmt_src}/cxxopts.hpp)

# target_include_directories(mqlqd_deps INTERFACE ${fmt_src}/include)
# target_include_directories(mqlqd_deps PUBLIC ${fmt_src}/include)
target_include_directories(mqlqd_deps PRIVATE ${fmt_src}/include)

# add_subdirectory(${fmt_src})

target_link_libraries(mqlqd_deps libfmtd.a)
target_link_libraries(mqlqd_deps -L${fmt_sub})

# target_link_libraries(mqlqd_deps PRIVATE fmt::fmt)

