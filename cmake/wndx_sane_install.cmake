## install function from wndx_sane lib.

function(wndx_sane_install) ## args
  cmake_parse_arguments(arg # pfx
    "" # opt
    "" # ovk
    "TARGETS;PATTERNS" # mvk
    ${ARGN}
  )
  message(DEBUG "PATTERNS: ${arg_PATTERNS}, TARGETS: ${arg_TARGETS}")
  if(arg_UNPARSED_ARGUMENTS)
    message(WARNING "UNPARSED: wndx_sane_install() ${arg_UNPARSED_ARGUMENTS}")
  endif()
  if(arg_KEYWORDS_MISSING_VALUES)
    message(WARNING " MISSING: wndx_sane_install() ${arg_KEYWORDS_MISSING_VALUES}")
  endif()
  if(NOT arg_PATTERNS)
    set(arg_PATTERNS "*.hpp" "*.h" "*.hh")
    message(WARNING "wndx_sane_install() PATTERNS not provided => used by default: ${arg_PATTERNS}")
  endif()

  message(STATUS "Generating Install")
  include(CMakePackageConfigHelpers)
  include(GNUInstallDirs)

  set(proj_config "${PROJECT_NAME}-config")
  cmake_path(APPEND proj_includedir "include" "${PROJECT_NAME}")
  cmake_path(APPEND cmake_proj_config "cmake" "${proj_config}")
  cmake_path(APPEND bin_dir_proj_config "${PROJECT_BINARY_DIR}" "${proj_config}")
  cmake_path(APPEND install_libdir_cmake "${CMAKE_INSTALL_LIBDIR}" "cmake" "${PROJECT_NAME}")

  install(TARGETS ${arg_TARGETS}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )

  install(EXPORT ${PROJECT_NAME}-targets
    NAMESPACE   wndx::
    DESTINATION "${install_libdir_cmake}"
  )

  configure_package_config_file(
    "${cmake_proj_config}.cmake.in"
    "${bin_dir_proj_config}.cmake"
    INSTALL_DESTINATION "${install_libdir_cmake}"
  )
  install(FILES "${bin_dir_proj_config}.cmake"
    DESTINATION "${install_libdir_cmake}"
  )

  write_basic_package_version_file(
    "${bin_dir_proj_config}-version.cmake"
    COMPATIBILITY ExactVersion
  )

  install(FILES "${bin_dir_proj_config}-version.cmake"
    DESTINATION "${install_libdir_cmake}"
  )

  install(DIRECTORY "${proj_includedir}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "${arg_PATTERNS}"
  )
endfunction()

