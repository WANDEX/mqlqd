## cxxopts fetch
## for the command line options.

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)
FetchContent_MakeAvailable(cxxopts)

add_subdirectory(${PROJECT_BINARY_DIR}/_deps/cxxopts-src/include)

