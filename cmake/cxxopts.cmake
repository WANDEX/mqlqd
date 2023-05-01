## cxxopts fetch
## for the command line options.

cmake_path(APPEND dep_dir ${PROJECT_BINARY_DIR} "_deps")

cmake_path(APPEND cxxopts_dir ${dep_dir}     "cxxopts") # lib dir root
cmake_path(APPEND cxxopts_sub ${cxxopts_dir} "sub")
cmake_path(APPEND cxxopts_src ${cxxopts_dir} "src")
cmake_path(APPEND cxxopts_bin ${cxxopts_dir} "bin")

# find_package(cxxopts 3.1.1)
# if(NOT cxxopts_FOUND)
if(TRUE) # XXX: ^ will not work cxxopts has weird configuration...
  message(">> fetching cxxopts of required version!")
  include(FetchContent)
  FetchContent_Declare(cxxopts
    GIT_REPOSITORY    https://github.com/jarro2783/cxxopts.git
    GIT_TAG           v3.1.1
    SUBBUILD_DIR      ${cxxopts_sub}
    SOURCE_DIR        ${cxxopts_src}
    BINARY_DIR        ${cxxopts_bin}
  )
  # option(CXXOPTS_ENABLE_INSTALL "" ON)
  FetchContent_MakeAvailable(cxxopts)
else()
  message(">> found cxxopts of required version!")
endif()

target_include_directories(mqlqd_deps PUBLIC "${cxxopts_src}/include")
target_precompile_headers(mqlqd_deps PRIVATE cxxopts.hpp)

