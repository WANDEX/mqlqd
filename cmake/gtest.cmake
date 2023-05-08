## XXX: prefer googletest version installed in the system || fetch if not found

cmake_path(APPEND dep_dir ${PROJECT_BINARY_DIR} "_deps")

cmake_path(APPEND gtest_dir ${dep_dir} "gtest") # lib dir root
cmake_path(APPEND gtest_sub ${gtest_dir} "sub")
cmake_path(APPEND gtest_src ${gtest_dir} "src")
cmake_path(APPEND gtest_bin ${gtest_dir} "bin")

# add_library(tests_deps "")
# add_library(tests_deps INTERFACE)
# add_library(tests_deps SHARED)

include(FindPkgConfig)
# pkg_search_module(GTEST gtest_main>=1.13.0)
# pkg_search_module(GTEST gtest_main=1.12.1) # XXX
find_package(GTest 1.13.0)
# find_package(GTest 1.12.1 EXACT)
if(GTEST_FOUND)
  message(">> found gtest.")
  # find_package(GTest 1.13.0) # XXX
else()
  message(">> fetch gtest.")
  include(FetchContent)
  FetchContent_Declare(googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           v1.13.0
    # GIT_TAG           release-1.12.1
    SUBBUILD_DIR      ${gtest_sub}
    SOURCE_DIR        ${gtest_src}
    BINARY_DIR        ${gtest_bin}
    FIND_PACKAGE_ARGS NAMES GTest
    # OVERRIDE_FIND_PACKAGE
  )
  option(BUILD_GMOCK   "" OFF)
  option(INSTALL_GTEST "" OFF)
  FetchContent_MakeAvailable(googletest)


  # set(PKG_CONFIG_ALLOW_SYSTEM_CFLAGS=yes)
  # set(PKG_CONFIG_ALLOW_SYSTEM_LIBS=yes)

  # set(PKG_CONFIG_ALLOW_SYSTEM_CFLAGS=no)
  # set(PKG_CONFIG_ALLOW_SYSTEM_LIBS=no)


  # target_include_directories(tests_deps PUBLIC "${gtest_src}/googlemock/include")
  # target_include_directories(tests_deps PUBLIC "${gtest_src}/googletest/include")
  # target_include_directories(tests_deps PUBLIC "${gtest_src}/googletest/include")

  ## link with the static library libfmtd.a
  ## which is found in the fetched locally lib dir.
  # target_link_libraries(tests_deps PRIVATE -L"${gtest_bin}/googletest" -lgtest_main)
  # target_link_libraries(tests_deps PUBLIC -L"${gtest_bin}/googletest" -lgtest_main)
  # target_link_libraries(tests_deps -L"${gtest_bin}/googletest" -lgtest_main)
  # target_link_libraries(tests_deps -L"${gtest_bin}/googletest" -lgtest -lgtest_main -lpthread)

  # include(GoogleTest)
endif()

include(GoogleTest)
## For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

# add_library(tests_deps INTERFACE)
# add_library(tests_deps SHARED "")

# target_link_libraries(tests_deps ${GTEST_LDFLAGS})
# target_compile_options(tests_deps PUBLIC ${GTEST_CFLAGS})

# target_link_libraries(tests_deps PUBLIC ${GTEST_LDFLAGS})
# target_compile_options(tests_deps PUBLIC ${GTEST_CFLAGS})

# target_link_libraries(tests_deps PUBLIC GTest::gtest_main)

