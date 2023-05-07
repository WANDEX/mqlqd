## XXX: prefer googletest version installed in the system || fetch if not found

cmake_path(APPEND dep_dir ${PROJECT_BINARY_DIR} "_deps")

cmake_path(APPEND gtest_dir ${dep_dir} "gtest") # lib dir root
cmake_path(APPEND gtest_sub ${gtest_dir} "sub")
cmake_path(APPEND gtest_src ${gtest_dir} "src")
cmake_path(APPEND gtest_bin ${gtest_dir} "bin")

# find_package(GTest 1.12.1)

# find_package(PkgConfig)
# if(NOT GTest_FOUND)
include(FindPkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main>=1.13.0)
if(${GTEST_MODULE_NAME} STREQUAL "")
# if(TRUE)
  message(">> fetch gtest.")
  include(FetchContent)
  FetchContent_Declare(googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           v1.13.0
    SUBBUILD_DIR      ${gtest_sub}
    SOURCE_DIR        ${gtest_src}
    BINARY_DIR        ${gtest_bin}
  )
  FetchContent_MakeAvailable(googletest)
else()
  message(">> found gtest.")
endif()

## For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(GoogleTest)
enable_testing()

# add_library(tests_deps INTERFACE)
# add_library(tests_deps SHARED "")
add_library(tests_deps "")
target_link_libraries(tests_deps ${GTEST_LDFLAGS})
target_compile_options(tests_deps PUBLIC ${GTEST_CFLAGS})

