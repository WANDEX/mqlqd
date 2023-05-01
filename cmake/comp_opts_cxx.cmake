## file with CXX compiler options.
##
## vars & funcs defined and come from the funcs.cmake
include(cmake/funcs.cmake) # useful project functions / variables
include(GNUInstallDirs)


function(add_alias name target)
  add_library(wandex::${name} ALIAS ${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${name})
endfunction()


get_filename_component(mqlqd_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include REALPATH)

get_filename_component(mqlqd_include_dir_mqlqd  ${mqlqd_include_dir}/mqlqd   REALPATH)

file(GLOB_RECURSE mqlqd_headers_mqlqd  LIST_DIRECTORIES false ${mqlqd_include_dir_mqlqd}  *.hpp PARENT_SCOPE)

## recursively include all sub-directories of the given dir
file(GLOB_RECURSE mqlqd_include_dirs LIST_DIRECTORIES true ${mqlqd_include_dir}/$)
list(FILTER       mqlqd_include_dirs EXCLUDE REGEX "/\\.") # exclude paths with .dirs
## ^ to allow using short includes (for internal library development only)

## uncomment to see which dirs are included
# foreach(dir ${mqlqd_include_dirs})
#   message(${dir})
# endforeach()

## Base target for common options.
add_library(_mqlqd_base INTERFACE ${mqlqd_headers_mqlqd})
target_include_directories(_mqlqd_base INTERFACE
  $<BUILD_INTERFACE:${mqlqd_include_dir}>
)
target_compile_features(_mqlqd_base INTERFACE cxx_std_20)


## Core library.
add_library(mqlqd_core INTERFACE)
add_alias(mqlqd::core mqlqd_core)
target_link_libraries(mqlqd_core INTERFACE _mqlqd_base)
target_include_directories(mqlqd_core SYSTEM INTERFACE
  $<BUILD_INTERFACE:${mqlqd_include_dir}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


## Core library with warnings; for development only.
add_library(mqlqd_dev INTERFACE)
add_alias(mqlqd::dev mqlqd_dev)
target_link_libraries(mqlqd_dev INTERFACE _mqlqd_base)
target_include_directories(mqlqd_dev INTERFACE
  $<BUILD_INTERFACE:${mqlqd_include_dirs}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


## here we set flags/options common to our main target compilers
if(${GNU_COMP} OR ${Clang_COMP} OR ${AppleClang_COMP})

  if(CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_options(mqlqd_dev INTERFACE -O3)
  elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(mqlqd_dev INTERFACE -g -Og)

    ## This helps to see/fix errors (which MSVC will throw anyway)
    ## => they should be fixed. (it is crucial flag, but has its own cost)
    target_compile_options(mqlqd_dev INTERFACE -D_GLIBCXX_DEBUG)
  endif()

  target_compile_options(mqlqd_dev INTERFACE -Wall -Wextra -Wpedantic -pedantic-errors)

  ## disallow implicit conversions
  ## NOTE: with -Wconversion Clang also enables -Wsign-conversion (GCC not!)
  target_compile_options(mqlqd_dev INTERFACE
    -Wconversion
    -Wsign-conversion
    -Wenum-conversion
    -Wfloat-conversion

    -Wsign-promo
    -Wdouble-promotion
  )

  target_compile_options(mqlqd_dev INTERFACE
    -Wold-style-cast
    -Wundef
    -Wshadow
    -ftrapv # XXX: sane default or delete for the "better"?
  )

  target_compile_options(mqlqd_dev INTERFACE
    -fdiagnostics-color=always
    -fdiagnostics-show-template-tree
  )

  ## enable this flags depending on the situation / debugging approach
  target_compile_options(mqlqd_dev INTERFACE
    # -Wfatal-errors
  )

  ## credit: https://gavinchou.github.io/experience/summary/syntax/gcc-address-sanitizer/
  ## TODO: remove this if clause as it is not finished!
  if(MQLQD_SNTZ_ADDR)
    message(NOTICE ">> ADDRESS SANITIZER ENABLED")
    target_compile_options(mqlqd_dev INTERFACE
      -ggdb -fno-omit-frame-pointer # call stack and line number report format
      # -fsanitize=address
      # -static-libstdc++
    )
    ## XXX FIXME linker or compile options?
    ## XXX This command cannot be used to add options for static library targets
    ## XXX ???
    target_link_options(mqlqd_dev INTERFACE
      -ggdb -fno-omit-frame-pointer # call stack and line number report format
      -fsanitize=address
      -static-libstdc++
    )
    ## platform specific:
    if(APPLE)
      target_compile_options(mqlqd_dev INTERFACE
        -static-libgcc # macos
      )
    elseif(UNIX)
      ## -lrt, needed by linux shared memory operation: shm_open and shm_unlink
      # target_compile_options(mqlqd_dev INTERFACE
      # target_link_options(mqlqd_dev INTERFACE
      #   -static-libasan -lrt # linux
      # )
    elseif(WIN32)
      # TODO: what to use here...
      target_add_check_cxx_compiler_flag(mqlqd_dev -static-libgcc ) # XXX works?
    endif()
  endif(MQLQD_SNTZ_ADDR)

endif()

if(MSVC)
  ## TODO: mimic all other flags from the targeted compilers
  ## (to have equal warnings between compilers and all environments/platforms)
  target_compile_options(mqlqd_dev INTERFACE /W3)

else()
  ## ^ (flag is obviously missing in MSVC if flag has leading - sign)
  ## Other flags which may miss in any of the targeted compilers.
  ## Not targeted compilers may have support of the GNU/Clang flags
  ## -> so we check support of the following flags, applying only supported.

  ### following flags are missing in Clang

  target_add_check_cxx_compiler_flag(mqlqd_dev -Warith-conversion )
  target_add_check_cxx_compiler_flag(mqlqd_dev -Wstrict-null-sentinel )
  target_add_check_cxx_compiler_flag(mqlqd_dev -Wzero-as-null-pointer-constant ) # has

  ### section for the other flags (may be or may be missing in Clang)
  ### for brevity - flags for the other compilers should be here

endif()

## Interface target
add_library(mqlqd_bin_i INTERFACE)
target_link_libraries(mqlqd_bin_i INTERFACE wandex::mqlqd::dev)
## link with the libc -lc (to work with the socket API)
target_link_libraries(mqlqd_bin_i INTERFACE c)

## Dependencies target
# add_library(mqlqd_deps)
target_link_libraries(mqlqd_deps PRIVATE wandex::mqlqd::core)
target_link_libraries(mqlqd_deps PRIVATE fmt::fmt)

## Umbrella target with all components.
add_library(mqlqd INTERFACE)
add_alias(mqlqd mqlqd)
target_link_libraries(mqlqd INTERFACE wandex::mqlqd::core)

