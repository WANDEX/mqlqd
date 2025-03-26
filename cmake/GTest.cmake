## GTest find/fetch from wndx_sane lib.

## BEG CONF
set(pkg_name "GTest")
set(pkg_repo "https://github.com/google/googletest.git")
set(pkg_tag "release-1.12.1")
set(pkg_ver "1.12.1")
set(pkg_tgt "${pkg_name}::gtest_main") ## target

option(BUILD_GMOCK "" OFF)
option(INSTALL_GTEST "" OFF)

## For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
## END CONF

include(cmake/wndx_sane_find.cmake)
wndx_sane_find(
  PKG_NAME  "${pkg_name}"
  PKG_REPO  "${pkg_repo}"
  PKG_TAG   "${pkg_tag}"
  PKG_VER   "${pkg_ver}"
  PKG_TGT   "${pkg_tgt}"
  FORCE_FETCH
  # PKG_NO_INCL
  PKG_NO_LINK
)

