## fmt fetch from wndx_sane lib.

## BEG CONF
set(pkg_name "fmt")
set(pkg_ver "9.1.0")
# set(pkg_ver "11.1.4")
set(pkg_url "https://github.com/fmtlib/fmt.git")
set(pkg_tgt "${pkg_name}::${pkg_name}") ## target

option(FMT_INSTALL "" ON)
option(FMT_OS "" OFF)
## END CONF

unset(dep_dir)
if(WNDX_SANE_DIR_3RDPARTY)
  if(IS_ABSOLUTE "${WNDX_SANE_DIR_3RDPARTY}" AND EXISTS "${WNDX_SANE_DIR_3RDPARTY}")
    message(">> dir for the 3rdparty dependency '${pkg_name}':")
    message(">> ${WNDX_SANE_DIR_3RDPARTY}")
    cmake_path(APPEND dep_dir "${WNDX_SANE_DIR_3RDPARTY}")
  else()
    cmake_path(APPEND dep_dir "${CMAKE_SOURCE_DIR}" ".3rdparty")
  endif()
else()
  cmake_path(APPEND dep_dir "${PROJECT_BINARY_DIR}" "_deps")
endif()

foreach(variable IN LISTS pkg_dir pkg_sub pkg_src pkg_bin pkg_inc)
  unset(${variable})
endforeach()
cmake_path(APPEND pkg_dir "${dep_dir}" "${pkg_name}") # lib dir root
cmake_path(APPEND pkg_sub "${pkg_dir}" "sub")
cmake_path(APPEND pkg_src "${pkg_dir}" "src")
cmake_path(APPEND pkg_bin "${pkg_dir}" "bin")
cmake_path(APPEND pkg_inc "${pkg_src}" "include")

set(${pkg_name}_DIR "${pkg_bin}")

find_package("${pkg_name}" "${pkg_ver}"
  PATHS "${pkg_bin}"
  NO_DEFAULT_PATH
)
if(NOT ${pkg_name}_FOUND)
  message(">> fetch ${pkg_name} of required version ${pkg_ver}")
  include(FetchContent)
  FetchContent_Declare(   "${pkg_name}"
    GIT_REPOSITORY        "${pkg_url}"
    GIT_TAG               "${pkg_ver}"
    SUBBUILD_DIR          "${pkg_sub}"
    SOURCE_DIR            "${pkg_src}"
    BINARY_DIR            "${pkg_bin}"
    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_MakeAvailable("${pkg_name}")
else()
  message(">> found ${pkg_name} of required version ${pkg_ver}")
endif()

## link with the static library fmt
## which is found in the fetched locally lib dir.
## NOTE: this is the same stuff, left for reference.
if(TRUE)
  if(TRUE)
    target_link_libraries(wndx_sane_deps "${pkg_tgt}")
  else()
    target_include_directories(wndx_sane_deps PUBLIC "${pkg_inc}")
    target_link_libraries(wndx_sane_deps PRIVATE "${pkg_tgt}")
  endif()
else()
  target_include_directories(wndx_sane_deps PUBLIC "${pkg_inc}")
  target_link_libraries(wndx_sane_deps PRIVATE
    -L"${pkg_bin}" -l$<$<CONFIG:Debug>:fmtd>$<$<CONFIG:Release>:fmt>
  )
endif()

