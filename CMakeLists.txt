cmake_minimum_required(VERSION 3.23)

## ^ POLICIES
project(
  mqlqd
  VERSION       0.0.1
  LANGUAGES     CXX
  DESCRIPTION   "File transfer over TCP/IP on sockets (POSIX) [client -> server]"
  HOMEPAGE_URL  "https://github.com/WANDEX/mqlqd"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(targets_and_linking)
  ## library targets & compilation options/flags
  include(cmake/wndx_sane_create_targets.cmake)
  wndx_sane_create_targets(
    CXX_STD cxx_std_20
    HPATH "include" "mqlqd"
  )

  ## link core library with the libc -lc (to work with the BSD API: socket, signal)
  # target_link_libraries(wndx_sane_core INTERFACE c)

  ## link sources with project dependencies from the core library target.
  target_link_libraries(wndx_sane_src INTERFACE wndx::wndx_sane::core)

  ## link sources with project dependencies
  target_link_libraries(wndx_sane_src PUBLIC wndx::wndx_sane::deps)

  ## link sources with project dev interface (for the compilation flags/options)
  target_link_libraries(wndx_sane_src PRIVATE wndx::wndx_sane::dev)

  ## find/fetch dependecies:
  if(MQLQD_BUILD_SRC)
    include(cmake/cxxopts.cmake)
    include(cmake/fmt.cmake)
    add_subdirectory(src)
  endif(MQLQD_BUILD_SRC)
endfunction(targets_and_linking)

## GUARD: check the real root of the source tree.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MQLQD_REAL_ROOT_OF_SRC_TREE ON)
else()
  set(MQLQD_REAL_ROOT_OF_SRC_TREE OFF)
  targets_and_linking()
endif()

if(MQLQD_REAL_ROOT_OF_SRC_TREE)
  option(MQLQD_BUILD_SRC        "whether or not src should be built"          ON)
  option(MQLQD_BUILD_TESTS      "whether or not tests should be built"        OFF) # TODO: UNIMPLEMENTED
  option(MQLQD_BUILD_PACKAGE    "whether or not the package should be built"  ON)
  option(MQLQD_COVERAGE_ENABLE  "whether or not to enable the tests coverage" OFF)
  option(MQLQD_COVERAGE_CLEAN   "clean coverage data before taking new"       ON)
  option(MQLQD_INSTALL_ENABLE   "whether or not to enable the install rule"   ON)
  option(MQLQD_MEMCHECK_ENABLE  "detect leaks via valgrind memcheck tool"     OFF)
  option(MQLQD_SNTZ_ADDR        "whether or not to enable address sanitizer"  OFF) # TODO: UNIMPLEMENTED

  ## For the list of supported compilers visit:
  ## https://cmake.org/cmake/help/latest/prop_tgt/COMPILE_WARNING_AS_ERROR.html
  option(CMAKE_COMPILE_WARNING_AS_ERROR "treat compilation warnings as errors" OFF)

  ## output dir is correct by default for most build setups.
  ## however, when building lib as a DLL, it is important to have the DLL in the same dir
  ## as the executable using it. Thus, we put all executables in a single /bin dir.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  ## On macOS, property ignored for the linker import files
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

  targets_and_linking()

  if(MQLQD_BUILD_TESTS)
    include(cmake/gtest.cmake)
    add_subdirectory(tests/units)
  endif(MQLQD_BUILD_TESTS)

  if(MQLQD_BUILD_PACKAGE)
    set(package_files include/ src/ cmake/ CMakeLists.txt LICENSE)
    set(fpath_zip ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip)
    add_custom_command(
      OUTPUT  ${fpath_zip}
      COMMAND ${CMAKE_COMMAND} -E tar c ${fpath_zip} --format=zip -- ${package_files}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${package_files}
    )
    add_custom_target(${PROJECT_NAME}_package DEPENDS ${fpath_zip})
  endif(MQLQD_BUILD_PACKAGE)

  if(MQLQD_COVERAGE_ENABLE)
    if(${GNU_COMP})
      add_library(code_coverage INTERFACE)
      add_dependencies(code_coverage tests_units)
      ## NOTE: without optimizations
      ## => other optimization levels/options obviously ruin coverage report.
      target_compile_options(code_coverage INTERFACE -g -O0 --coverage)
      target_link_options(code_coverage INTERFACE --coverage)

      if(MQLQD_COVERAGE_CLEAN)
        ### before taking new coverage analysis data:
        ## find all coverage files in the project binary dir
        file(GLOB_RECURSE prev_cov_files LIST_DIRECTORIES false
          ABSOLUTE "${CMAKE_BINARY_DIR}" *.gcda) # XXX *.gcno
        ## clean from the previous coverage data files (if any)
        if(NOT prev_cov_files STREQUAL "")
          file(REMOVE ${prev_cov_files})
          message(NOTICE ">> Cleared of previous coverage data files.")
        else()
          message(NOTICE ">> Nothing to clean up. (no coverage data files found)")
        endif()
      endif()

      message(NOTICE ">> Code Coverage will be collected!")
    else()
      message(WARNING "Code Coverage analysis enabled only on the GNU/GCC toolchain!")
    endif()
  endif(MQLQD_COVERAGE_ENABLE)

  if(MQLQD_MEMCHECK_ENABLE)
    add_custom_target(memcheck
      COMMAND valgrind --tool=memcheck -s --leak-check=full --show-leak-kinds=all
                      --error-exitcode=73 # --exit-on-first-error=yes
                      -- ./tests_units --gtest_brief=1
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      DEPENDS tests_units
    )
    add_dependencies(memcheck tests_units)
  endif(MQLQD_MEMCHECK_ENABLE)

  if(MQLQD_INSTALL_ENABLE)
    include(cmake/wndx_sane_install.cmake)
    wndx_sane_install(
      TARGETS mqlqd_client mqlqd_daemon
        _wndx_sane_base wndx_sane_core wndx_sane_dev
      PATTERNS "*.hpp"
    )
  endif(MQLQD_INSTALL_ENABLE)
endif(MQLQD_REAL_ROOT_OF_SRC_TREE)

