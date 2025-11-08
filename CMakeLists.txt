cmake_minimum_required(VERSION 3.29..4.1)

## ^ POLICIES
project(
  mqlqd
  VERSION       0.3.0
  LANGUAGES     CXX
  DESCRIPTION   "File transfer over TCP/IP on sockets (POSIX) [client -> server]"
  HOMEPAGE_URL  "https://github.com/WANDEX/mqlqd"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MQLQD_DISABLE_TOP_PROJECT OFF)
if(PROJECT_IS_TOP_LEVEL AND NOT MQLQD_DISABLE_TOP_PROJECT)
  set(MQLQD_IS_TOP_PROJECT ON)
else()
  set(MQLQD_IS_TOP_PROJECT OFF)
endif()

## search path for cmake modules to be loaded by: include() find_package().
## NOTE: works properly only if defined at the top-level CMakeLists.txt.
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ## project modules
  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wndx/sane/cmake"
)

function(targets_and_linking)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wndx/sane")
  include(wndx_sane_create_targets)
  ## library targets & compilation options/flags
  wndx_sane_create_targets(PFX wndx LIB mqlqd
    CXX_STD cxx_std_20
    PHD "include/mqlqd"
  )

  ## inherit from wndx::sane targets
  target_link_libraries(mqlqd_core INTERFACE wndx::sane::core) # inherit core
  target_link_libraries(mqlqd_dev  INTERFACE wndx::sane::dev ) # inherit dev
  target_link_libraries(mqlqd_deps    PUBLIC wndx::sane::deps) # inherit deps
  target_link_libraries(mqlqd_src     PUBLIC wndx::sane::src ) # inherit src

  ## link sources with project dependencies from the core library target
  target_link_libraries(mqlqd_src INTERFACE wndx::mqlqd::core)

  ## link sources with project dependencies
  target_link_libraries(mqlqd_src PUBLIC wndx::mqlqd::deps)

  ## link sources with project dev interface (for the compilation flags/options)
  target_link_libraries(mqlqd_src PRIVATE wndx::mqlqd::dev)

  if(MQLQD_BUILD_SRC OR NOT MQLQD_IS_TOP_PROJECT)
    ## find/fetch dependecies:
    find_package(cxxopts REQUIRED)
    find_package(fmt REQUIRED)
    add_subdirectory(src/mqlqd)
  endif()
endfunction(targets_and_linking)

if(NOT MQLQD_IS_TOP_PROJECT)
  targets_and_linking()
endif()

if(MQLQD_IS_TOP_PROJECT)
  option(MQLQD_BUILD_SRC        "whether or not src should be built"          ON)
  option(MQLQD_BUILD_TESTS      "whether or not tests should be built"        OFF)
  option(MQLQD_BUILD_PACKAGE    "whether or not the package should be built"  ON)
  option(MQLQD_COVERAGE_ENABLE  "whether or not to enable the tests coverage" OFF)
  option(MQLQD_COVERAGE_CLEAN   "clean coverage data before taking new"       ON)
  option(MQLQD_INSTALL_ENABLE   "whether or not to enable the install rule"   ON)
  option(MQLQD_MEMCHECK_ENABLE  "detect leaks via valgrind memcheck tool"     OFF)

  ## for the list of supported compilers visit:
  ## https://cmake.org/cmake/help/latest/prop_tgt/COMPILE_WARNING_AS_ERROR.html
  option(CMAKE_COMPILE_WARNING_AS_ERROR "treat compilation warnings as errors" OFF)

  ## output dir is correct by default for most build setups.
  ## however, when building lib as a DLL, it is important to have the DLL in the same dir
  ## as the executable using it. Thus, we put all executables in a single /bin dir.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  ## on macOS, property ignored for the linker import files
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

  targets_and_linking()

  if(MQLQD_COVERAGE_ENABLE)
    include(wndx_sane_coverage)
    wndx_sane_coverage(TGT_NAME code_coverage
      CLEAN MQLQD_COVERAGE_CLEAN
    )
  endif(MQLQD_COVERAGE_ENABLE)

  if(MQLQD_BUILD_TESTS)
    add_subdirectory(tests/units)
  endif(MQLQD_BUILD_TESTS)

  if(MQLQD_BUILD_PACKAGE)
    include(wndx_sane_package)
    wndx_sane_package(SUFFIX src FILES
      cmake/ include/ scripts/ src/ tests/ CMakeLists.txt LICENSE README
    )
  endif(MQLQD_BUILD_PACKAGE)

  if(MQLQD_MEMCHECK_ENABLE)
    include(wndx_sane_memcheck)
    wndx_sane_memcheck(TGT_NAME memcheck
      TGT_EXEC tests_units --gtest_brief=1
      # VALGRIND_OPTS --read-var-info=yes --show-leak-kinds=definite --track-origins=yes
      # DRMEMORY_OPTS -brief -no_crash_at_error
      # EXIT_ON_FIRST_ERROR
      # FORCE_DRMEMORY
    )
  endif(MQLQD_MEMCHECK_ENABLE)

  if(MQLQD_INSTALL_ENABLE)
    include(wndx_sane_install)
    wndx_sane_install(
      TARGETS mqlqd_client mqlqd_daemon
      PHD "include/mqlqd"
      PATTERNS "*.hpp"
    )
  endif(MQLQD_INSTALL_ENABLE)
endif(MQLQD_IS_TOP_PROJECT)
