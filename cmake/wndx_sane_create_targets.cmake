## create default targets
## create dev target with strict CXX compiler options for development.

## vars & funcs defined and come from the funcs.cmake
include(cmake/funcs.cmake) # useful project functions / variables

function(add_alias name target)
  add_library(wndx::${name} ALIAS ${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${name})
endfunction()

function(wndx_sane_path arg_VAR_NAME) ## ARGN
  unset(${arg_VAR_NAME} PARENT_SCOPE) ## unset old variable
  file(REAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" args)
  cmake_path(APPEND args ${ARGN})
  set(${arg_VAR_NAME} "${args}" PARENT_SCOPE) ## set literal variable
  # message("${ARGN}")
  # message("${arg_VAR_NAME}")
endfunction()

function(wndx_sane_create_targets) ## args
  cmake_parse_arguments(arg # pfx
    "" # opt
    "CXX_STD" # ovk
    "HPATH" # mvk
    ${ARGN}
  )
  message(DEBUG "CXX_STD: ${arg_CXX_STD}, HPATH: ${arg_HPATH}")
  if(arg_UNPARSED_ARGUMENTS)
    message(WARNING "UNPARSED: wndx_sane_create_targets() ${arg_UNPARSED_ARGUMENTS}")
  endif()
  if(arg_KEYWORDS_MISSING_VALUES)
    message(WARNING " MISSING: wndx_sane_create_targets() ${arg_KEYWORDS_MISSING_VALUES}")
  endif()
  if(NOT arg_CXX_STD MATCHES "^cxx_std_..$")
    set(arg_CXX_STD cxx_std_20)
    message(WARNING "wndx_sane_create_targets() CXX_STD not provided => used by default: ${arg_CXX_STD}")
  endif()

  wndx_sane_path(WNDX_SANE_PATH_TMP "${arg_HPATH}")
  file(GLOB_RECURSE wndx_sane_headers LIST_DIRECTORIES false "${WNDX_SANE_PATH_TMP}" *.hpp, *.h)

  ## Base target for common options.
  add_library(_wndx_sane_base INTERFACE)
  target_include_directories(_wndx_sane_base INTERFACE
    $<BUILD_INTERFACE:${WNDX_SANE_PATH_TMP}>
  )
  target_compile_features(_wndx_sane_base INTERFACE ${arg_CXX_STD})

  ## target for dependencies (link inheritance)
  add_library(wndx_sane_deps "")
  add_alias(wndx_sane::deps wndx_sane_deps)
  target_include_directories(wndx_sane_deps PUBLIC
    $<BUILD_INTERFACE:${WNDX_SANE_PATH_TMP}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  target_sources(wndx_sane_deps PUBLIC ${wndx_sane_headers})
  set_target_properties(wndx_sane_deps PROPERTIES LINKER_LANGUAGE CXX)
  target_compile_features(wndx_sane_deps PUBLIC ${arg_CXX_STD})

  ## target for src (link inheritance)
  add_library(wndx_sane_src "")
  add_alias(wndx_sane::src wndx_sane_src)
  target_include_directories(wndx_sane_src PUBLIC
    $<BUILD_INTERFACE:${WNDX_SANE_PATH_TMP}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  target_sources(wndx_sane_src PUBLIC ${wndx_sane_headers})
  set_target_properties(wndx_sane_src PROPERTIES LINKER_LANGUAGE CXX)
  target_compile_features(wndx_sane_src PUBLIC ${arg_CXX_STD})

  ## Core library.
  add_library(wndx_sane_core INTERFACE)
  add_alias(wndx_sane::core wndx_sane_core)
  target_link_libraries(wndx_sane_core INTERFACE _wndx_sane_base)
  target_include_directories(wndx_sane_core SYSTEM INTERFACE
    $<BUILD_INTERFACE:${WNDX_SANE_PATH_TMP}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  ## Core library with warnings; for development only.
  add_library(wndx_sane_dev INTERFACE)
  add_alias(wndx_sane::dev wndx_sane_dev)
  target_link_libraries(wndx_sane_dev INTERFACE _wndx_sane_base)
  target_include_directories(wndx_sane_dev INTERFACE
    $<BUILD_INTERFACE:${WNDX_SANE_PATH_TMP}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  ## Umbrella target with all components.
  # add_library(wndx_sane INTERFACE)
  # add_alias(wndx_sane wndx_sane)
  # target_link_libraries(wndx_sane INTERFACE wndx::wndx_sane::core)

  ## here we set flags/options common to our main target compilers
  if(${GNU_COMP} OR ${Clang_COMP} OR ${AppleClang_COMP})

    if(CMAKE_BUILD_TYPE STREQUAL Release)
      target_compile_options(wndx_sane_dev INTERFACE -O3)
    elseif(CMAKE_BUILD_TYPE STREQUAL Debug)
      target_compile_options(wndx_sane_dev INTERFACE -g -Og)

      ## This helps to see/fix errors (which MSVC will throw anyway)
      ## => they should be fixed. (it is crucial flag, but has its own cost)
      target_compile_options(wndx_sane_dev INTERFACE -D_GLIBCXX_DEBUG)
    endif()

    target_compile_options(wndx_sane_dev INTERFACE -Wall -Wextra -Wpedantic -pedantic-errors)

    ## disallow implicit conversions
    ## NOTE: with -Wconversion Clang also enables -Wsign-conversion (GCC not!)
    target_compile_options(wndx_sane_dev INTERFACE
      -Wconversion
      -Wsign-conversion
      -Wenum-conversion
      -Wfloat-conversion

      -Wsign-promo
      -Wdouble-promotion
    )

    target_compile_options(wndx_sane_dev INTERFACE
      -Wold-style-cast
      -Wundef
      -Wshadow
      -ftrapv
    )

    target_compile_options(wndx_sane_dev INTERFACE
      -fdiagnostics-color=always
      -fdiagnostics-show-template-tree
    )

    ## enable this flags depending on the situation / debugging approach
    target_compile_options(wndx_sane_dev INTERFACE
      # -Wfatal-errors
    )

    ## credit: https://gavinchou.github.io/experience/summary/syntax/gcc-address-sanitizer/
    ## TODO: remove this if clause as it is not finished!
    if(wndx_sane_SNTZ_ADDR)
      message(NOTICE ">> ADDRESS SANITIZER ENABLED")
      target_compile_options(wndx_sane_dev INTERFACE
        -ggdb -fno-omit-frame-pointer # call stack and line number report format
        # -fsanitize=address
        # -static-libstdc++
      )
      ## XXX FIXME linker or compile options?
      ## XXX This command cannot be used to add options for static library targets
      ## XXX ???
      target_link_options(wndx_sane_dev INTERFACE
        -ggdb -fno-omit-frame-pointer # call stack and line number report format
        -fsanitize=address
        -static-libstdc++
      )
      ## platform specific:
      if(APPLE)
        target_compile_options(wndx_sane_dev INTERFACE
          -static-libgcc # macos
        )
      elseif(UNIX)
        ## -lrt, needed by linux shared memory operation: shm_open and shm_unlink
        # target_compile_options(wndx_sane_dev INTERFACE
        # target_link_options(wndx_sane_dev INTERFACE
        #   -static-libasan -lrt # linux
        # )
      elseif(WIN32)
        # FIXME: what to use here...
        # target_add_check_cxx_compiler_flag(wndx_sane_dev /static-libgcc )
      endif()
    endif(wndx_sane_SNTZ_ADDR)

  endif()

  if(MSVC)
    ## TODO: mimic all other flags from the targeted compilers
    ## (to have equal warnings between compilers and all environments/platforms)
    target_compile_options(wndx_sane_dev INTERFACE /W3 /utf-8)

  else()
    ## ^ (flag is obviously missing in MSVC if flag has leading - sign)
    ## Other flags which may miss in any of the targeted compilers.
    ## Not targeted compilers may have support of the GNU/Clang flags
    ## -> so we check support of the following flags, applying only supported.

    ### following flags are missing in Clang

    target_add_check_cxx_compiler_flag(wndx_sane_dev -Warith-conversion )
    target_add_check_cxx_compiler_flag(wndx_sane_dev -Wstrict-null-sentinel )
    target_add_check_cxx_compiler_flag(wndx_sane_dev -Wzero-as-null-pointer-constant ) # has

    if(${GNU_COMP})
      ## gives many false positives with GCC 13
      ## https://github.com/fmtlib/fmt/issues/3415
      target_add_check_cxx_compiler_flag(wndx_sane_dev -Wno-dangling-reference )
    endif()

    ### section for the other flags (may be or may be missing in Clang)
    ### for brevity - flags for the other compilers should be here

  endif()

endfunction()
