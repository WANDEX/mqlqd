#pragma once

#include <fmt/format.h>


namespace wndx::mqlqd {

enum class rc // NOLINT(performance-enum-size)
{
  SUCCESS = 0,
  FAILURE,
  DUMMY_FIXME,

  INIT,

  MEMORY_BLOCK_EMPTY, // MEMORY_BLOCK_EMPTY
  MEMORY_BAD_ALLOC,   // "std::bad_alloc"

  FILE_FIXME,
  FILE_SPECIAL,
  FILE_STATUS_GENERIC_ERRO,
  FILE_MKDIR_GENERIC_ERRO,
  FILE_PATH_CANONICAL_ERRO,
  FILE_RM_DIR_EMPTY_SUCCESS,
  FILE_RM_DIR_PERMS_ERRO,
  FILE_PATH_UNEXPECTED_ERRO,

  FS_CTOR_ERRO,
  FS_OPEN_ERRO,
  FS_IO_UNAVAILABLE,

  UNIX_SOCK_MAKE_ERRO,
  UNIX_SOCK_ADDR_ERRO,
  UNIX_SOCK_BIND_ERRO,
  UNIX_SOCK_LSTN_ERRO,
  UNIX_SOCK_CONN_ERRO,
  UNIX_SOCK_SEND_ERRO,
  UNIX_SOCK_RECV_ERRO,

  WARN_CMD_FILE_REQ,

  ERRO_CMD_OPT,

  CRIT_EX_FIXME,
  CRIT_EX_UNHANDLED,
  CRIT_EX_ANONYMOUS,
};

} // namespace wndx::mqlqd

// fmt demands to specialize/declare this here!
// [template_spec_redecl_out_of_scope].
// @brief format enum SIG in the readable text form.
template <>
struct fmt::formatter<wndx::mqlqd::rc> : formatter<string_view>
{
  using RC = wndx::mqlqd::rc;
  template <typename FormatContext>
  auto format(RC rc, FormatContext& ctx) const
  {
    // clang-format off: readability - align & fit each on a single line.
    string_view what = "unknown";
    switch (rc) {
    case RC::SUCCESS:                     what = "SUCCESS"; break;
    case RC::FAILURE:                     what = "FAILURE"; break;
    case RC::DUMMY_FIXME:                 what = "DUMMY_FIXME"; break;

    case RC::INIT:                        what = "INIT"; break;

    case RC::MEMORY_BLOCK_EMPTY:          what = "MEMORY_BLOCK_EMPTY"; break;
    case RC::MEMORY_BAD_ALLOC:            what = "MEMORY_BAD_ALLOC"; break;

    case RC::FILE_FIXME:                  what = "FILE_FIXME"; break;
    case RC::FILE_SPECIAL:                what = "FILE_SPECIAL"; break;
    case RC::FILE_STATUS_GENERIC_ERRO:    what = "FILE_STATUS_GENERIC_ERRO"; break;
    case RC::FILE_MKDIR_GENERIC_ERRO:     what = "FILE_MKDIR_GENERIC_ERRO"; break;
    case RC::FILE_PATH_CANONICAL_ERRO:    what = "FILE_PATH_CANONICAL_ERRO"; break;
    case RC::FILE_RM_DIR_EMPTY_SUCCESS:   what = "FILE_RM_DIR_EMPTY_SUCCESS"; break;
    case RC::FILE_RM_DIR_PERMS_ERRO:      what = "FILE_RM_DIR_PERMS_ERRO"; break;
    case RC::FILE_PATH_UNEXPECTED_ERRO:   what = "FILE_PATH_UNEXPECTED_ERRO"; break;

    case RC::FS_CTOR_ERRO:                what = "FS_CTOR_ERRO"; break;
    case RC::FS_OPEN_ERRO:                what = "FS_OPEN_ERRO"; break;
    case RC::FS_IO_UNAVAILABLE:           what = "FS_IO_UNAVAILABLE"; break;

    case RC::UNIX_SOCK_MAKE_ERRO:         what = "UNIX_SOCK_MAKE_ERRO"; break;
    case RC::UNIX_SOCK_ADDR_ERRO:         what = "UNIX_SOCK_ADDR_ERRO"; break;
    case RC::UNIX_SOCK_BIND_ERRO:         what = "UNIX_SOCK_BIND_ERRO"; break;
    case RC::UNIX_SOCK_LSTN_ERRO:         what = "UNIX_SOCK_LSTN_ERRO"; break;
    case RC::UNIX_SOCK_CONN_ERRO:         what = "UNIX_SOCK_CONN_ERRO"; break;
    case RC::UNIX_SOCK_SEND_ERRO:         what = "UNIX_SOCK_SEND_ERRO"; break;
    case RC::UNIX_SOCK_RECV_ERRO:         what = "UNIX_SOCK_RECV_ERRO"; break;

    case RC::WARN_CMD_FILE_REQ:           what = "No files were provided"; break;

    case RC::ERRO_CMD_OPT     :           what = "Fail during parsing of the cmd options"; break;

    case RC::CRIT_EX_FIXME    :           what = "FIXME DUMMY exception"; break;
    case RC::CRIT_EX_UNHANDLED:           what = "UNHANDLED std::exception"; break;
    case RC::CRIT_EX_ANONYMOUS:           what = "UNHANDLED anonymous exception"; break;
    }
    return formatter<string_view>::format(what, ctx);
    // clang-format on
  }
};

// overload for the std::ostream (to print rc in the readable text form)
inline std::ostream& operator<<(std::ostream& os, wndx::mqlqd::rc rc)
{
  return os << fmt::to_string(rc);
}
