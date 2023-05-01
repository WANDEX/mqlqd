## cxxopts fetch
## for the command line options.

# find_package(cxxopts 3.1.1)
# if(NOT cxxopts_FOUND)
if(TRUE) # XXX: ^ will not work cxxopts has weird configuration...
  # message(">> fetching cxxopts of required version!")
  include(FetchContent)
  FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.1.1
  )
  FetchContent_MakeAvailable(cxxopts)
else()
  message(">> found cxxopts of required version!")
endif()

get_filename_component(cxxopts_incld
  ${PROJECT_BINARY_DIR}/_deps/cxxopts-src/include REALPATH
)
target_include_directories(mqlqd_deps PUBLIC ${cxxopts_incld})
target_precompile_headers(mqlqd_deps PRIVATE ${cxxopts_incld}/cxxopts.hpp)

