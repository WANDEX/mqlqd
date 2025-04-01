## cmake function to find/fetch from wndx_sane lib.

function(wndx_sane_find) ## args
  cmake_parse_arguments(arg # pfx
    "FORCE_FETCH;PKG_NO_LINK;PKG_NO_INCL" # opt
    "PKG_NAME;PKG_REPO;PKG_TAG;PKG_VER;PKG_TGT" # ovk
    "" # mvk
    ${ARGN}
  )
  message(DEBUG "PKG_NAME: ${arg_PKG_NAME}, PKG_VER: ${arg_PKG_VER}, PKG_TGT: ${arg_PKG_TGT}")
  message(DEBUG "PKG_REPO: ${arg_PKG_REPO}, PKG_TAG: ${arg_PKG_TAG}")
  if(arg_UNPARSED_ARGUMENTS)
    message(WARNING "UNPARSED: wndx_sane_find() ${arg_UNPARSED_ARGUMENTS}")
  endif()
  if(arg_KEYWORDS_MISSING_VALUES)
    message(FATAL_ERROR " MISSING: wndx_sane_find() ${arg_KEYWORDS_MISSING_VALUES}")
  endif()
  if(NOT arg_PKG_TGT MATCHES "^.*::.*$")
    message(FATAL_ERROR "wndx_sane_find() PKG_TGT not a valid target ${arg_PKG_TGT}")
  endif()

  set(pkg_name "${arg_PKG_NAME}") ## package name
  set(pkg_repo "${arg_PKG_REPO}") ## git repository
  set(pkg_tag  "${arg_PKG_TAG}") ## git tag
  set(pkg_ver  "${arg_PKG_VER}") ## find version
  set(pkg_tgt  "${arg_PKG_TGT}") ## package target

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

  ## so that the find_package will look at the specified dir first
  set(${pkg_name}_DIR "${pkg_bin}")

  find_package("${pkg_name}" "${pkg_ver}"
    PATHS "${pkg_bin}"
    NO_DEFAULT_PATH
  )
  if(NOT ${pkg_name}_FOUND OR ${arg_FORCE_FETCH})
    if(${arg_FORCE_FETCH})
      message(">> FORCE fetch ${pkg_name} of required version ${pkg_ver}")
    else()
      message(">> fetch ${pkg_name} of required version ${pkg_ver}")
    endif()
    include(FetchContent)
    FetchContent_Declare(   "${pkg_name}"
      GIT_REPOSITORY        "${pkg_repo}"
      GIT_TAG               "${pkg_tag}"
      SUBBUILD_DIR          "${pkg_sub}"
      SOURCE_DIR            "${pkg_src}"
      BINARY_DIR            "${pkg_bin}"
      OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable("${pkg_name}")
  else()
    message(">> found ${pkg_name} of required version ${pkg_ver}")
  endif()

  if(NOT ${arg_PKG_NO_LINK})
    if(${arg_PKG_NO_INCL})
      message(DEBUG ">> PKG_NO_INCL ${pkg_tgt}")
      target_link_libraries(wndx_sane_deps "${pkg_tgt}")
    else()
      target_include_directories(wndx_sane_deps PUBLIC "${pkg_inc}")
      target_link_libraries(wndx_sane_deps PRIVATE "${pkg_tgt}")
    endif()
  endif()
endfunction(wndx_sane_find)

