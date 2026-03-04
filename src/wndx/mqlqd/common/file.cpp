
#include "wndx/mqlqd/aliases.hpp" // IWYU pragma: keep

#include "wndx/mqlqd/file.hpp"
#include "wndx/mqlqd/rc.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error> // ec - error_code


// show boundaries of the files in the cat mode
// clang-format off
#ifndef MQLQD_FILE_CONTENTS_BOUNDARY
#define MQLQD_FILE_CONTENTS_BOUNDARY 0 // NOLINT(*-macro-usage)
#endif//MQLQD_FILE_CONTENTS_BOUNDARY
// clang-format on


namespace wndx::sane::path {

/// \brief trim trailing separators.
/// any path provided by the user may contain a trailing separator - trim it!
[[nodiscard]] fs::path trim(fs::path const& path) noexcept
{
  fs::path npath{ path };
  // additional trailing directory separators are never introduced
  npath += fs::path::preferred_separator;
  return npath.parent_path(); // safely remove trailing separators
}

[[nodiscard]] fs::path sanitize(fs::path const& path) noexcept
{
  return trim(path);
}

} // namespace wndx::sane::path

namespace wndx::mqlqd::file {

/**
 * @brief check that the file is regular file.
 *
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
[[nodiscard]] rc is_r(fs::path const& fpath) noexcept
{
  static constexpr auto fn{ "file::is_r()" };
  std::error_code       ec{};
  fs::file_status       s{ fs::status(fpath, ec) }; // for the noexcept
  if (ec) {
    WNDX_LOG(LL::CRIT, "{} {} fs::status error v:{} m:{}\n", fn, fpath.c_str(),
             ec.value(), ec.message());
    return rc::FILE_STATUS_GENERIC_ERRO;
  }
  if (!fs::is_regular_file(s)) {
    WNDX_LOG(LL::ERRO, "{} is NOT a regular file: {}\n", fn, fpath.c_str());
    return rc::FILE_SPECIAL;
  }
  WNDX_LOG(LL::INFO, "{} is a regular file: {}\n", fn, fpath.c_str());
  // TODO: check file permissions (maybe it is a good idea to have)
  return rc::SUCCESS;
}


/**
 * @brief make dir and set required perms. (should be safe)
 *
 * @param  new dir path.
 * @param  new dir permissions.
 * @param  force/insist on importance of the access rights to the dir.
 * @return 0 on success, else non zero return code.
 */
[[nodiscard]] rc mkdir(fs::path const& dpath, fs::perms const& perms,
                       bool force) noexcept
{
  static constexpr auto fn{ "file::mkdir()" };
  std::error_code       ec{};
  fs::path const        npath{ wndx::sane::path::trim(dpath) };
  fs::path const        parent_dir{ npath.parent_path() };
  if (fs::is_directory(npath, ec)) {
    WNDX_LOG(LL::DBUG, "{} Directory exist, and will be used as is\n", fn);
    if (force) {
      // TODO: validate that directory permissions are set as requested.
    }
  } else {
    if (!fs::exists(npath, ec)) { // expected branch
      if (fs::create_directory(npath, parent_dir, ec)) {
        fs::permissions(npath, perms, ec);
        if (ec.value() == 0) {
          WNDX_LOG(
              LL::NTFY,
              "{} Created new directory with requested access permissions.\n",
              fn);
        } else {
          WNDX_LOG(
              LL::ERRO,
              "{} Created new directory, but failed to set requested perms.\n",
              fn);
          if (fs::remove(npath, ec)) {
            WNDX_LOG(
                LL::NTFY,
                "{} Removed empty directory created with incorrect perms.\n",
                fn);
            return rc::FILE_RM_DIR_EMPTY_SUCCESS;
          } else {
            WNDX_LOG(LL::CRIT,
                     "{} {}\n^ failed to remove this dir\n"
                     "that was created with incorrect access rights.\n"
                     "Because of the perms, do it yourself manually!\n"
                     "Additionally take a look at the origin of the error:\n"
                     "v:{} m:{}\n",
                     fn, npath.c_str(), ec.value(), ec.message());
            return rc::FILE_RM_DIR_PERMS_ERRO;
          }
        }
      } else {
        WNDX_LOG(LL::CRIT,
                 "{} If your intent was to make this dir under current cwd:\n"
                 "{}\n^ precede your path with './' i.e. ./{}\n"
                 "Additionally take a look at the origin of the error:\n"
                 "fs::create_directory error:\nv:{} m:{}\n",
                 fn, npath.c_str(), npath.c_str(), ec.value(), ec.message());
        return rc::FILE_MKDIR_GENERIC_ERRO;
      }
    } else { // path already exist
      auto realpath{ fs::canonical(npath, ec) };
      if (ec) {
        WNDX_LOG(LL::CRIT, "{} {}\n^ canonical path error:\nv:{} m:{}\n", fn,
                 npath.c_str(), ec.value(), ec.message());
        return rc::FILE_PATH_CANONICAL_ERRO;
      }
      WNDX_LOG(LL::CRIT, "{} {}\n^ path cannot be used as the new directory!\n",
               fn, realpath.c_str());
      return rc::FILE_PATH_UNEXPECTED_ERRO;
    }
  }
  return rc::SUCCESS;
}


File::File(fs::path fpath, size_t sz) noexcept
    : m_block_size{ sz }
    , m_fpath{ std::move(fpath) }
{
  WNDX_LOG(LL::DBUG,
           "ctor - File instance from file & size:\n"
           "\t[{}] {}\n",
           m_block_size, m_fpath.c_str());
}

File::File(Finfo const& finfo, fs::path const& dpath) noexcept
    : m_block_size{ finfo.block_size }
    , m_fpath{ dpath / finfo.fname } // concatenate path
{
  WNDX_LOG(LL::DBUG, "ctor - File instance from Finfo:\n\t[{}] {}\n",
           m_block_size, m_fpath.c_str());
}

[[nodiscard]] Finfo File::to_finfo() const noexcept
{
  Finfo finfo{};
  finfo.block_size = m_block_size;
  // fill the array - file name (element by element)
  auto fname{ m_fpath.filename().string() };
  for (size_t i = 0; i < fname.length(); ++i) {
    finfo.fname[i] = fname[i]; // NOLINT(*-constant-array-index)
  }
  // null terminator after the file name (important)
  finfo.fname[fname.length()] = '\0'; // NOLINT(*-constant-array-index)
  return finfo;
}

[[nodiscard]] rc File::write() const noexcept
{
  static constexpr auto fn{ "File::write()" };
  if (!m_block) {
    WNDX_LOG(LL::CRIT, "{} {}\n", fn, fn, rc::MEMORY_BLOCK_EMPTY);
    return rc::MEMORY_BLOCK_EMPTY;
  }
  // open file for writing
  std::basic_fstream<char_type> ofs(m_fpath, openmode_w);
  if (!ofs) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_CTOR_ERRO);
    return rc::FS_CTOR_ERRO;
  }
  if (ofs.fail()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_OPEN_ERRO);
    return rc::FS_OPEN_ERRO;
  }
  if (!ofs.good()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_IO_UNAVAILABLE);
    return rc::FS_IO_UNAVAILABLE;
  }
  try {
    // NOTE: for loop is necessary for writing full contents of the binary file!
    for (size_t i = 0; i < m_block_size; i++) {
      ofs << m_block[i]; // NOLINT(*-pointer-arithmetic)
    }
    WNDX_LOG(LL::STAT, "{} SUCCESS:\n\t{}\n", fn, m_fpath.c_str());
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

// It is easy to use std::unique_ptr here for the mem_block
// but maybe there is the benefit in proper using of the raw pointers?
// It is easy to rewrite if requested!
[[nodiscard]] rc File::heap_alloc() noexcept
{
  static constexpr auto fn{ "File::heap_alloc()" };
  try {
    m_block = { new char_type[m_block_size]{} };
    if (!m_block) {
      WNDX_LOG(LL::CRIT, "{} {}\n", fn, rc::MEMORY_BLOCK_EMPTY);
      return rc::MEMORY_BLOCK_EMPTY;
    }
    WNDX_LOG(LL::DBUG, "{}\n", fn);
  } catch (std::bad_alloc const& err) {
    WNDX_LOG(LL::ERRO, "{} {}\n{}\n", fn, rc::MEMORY_BAD_ALLOC, err.what());
    File::~File();
    return rc::MEMORY_BAD_ALLOC;
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    File::~File();
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

void File::heap_cleanup() noexcept
{
  delete[] m_block;
  m_block = nullptr;
}

File::~File() noexcept { heap_cleanup(); }

/**
 * @brief read file contents into the memory block.
 *
 * @param  mem_block - memory block.
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
[[nodiscard]] rc File::fcontent() const noexcept
{
  static constexpr auto fn{ "File::fcontent()" };
  rc                    rc{ is_r(m_fpath) }; // return code
  if (rc != rc::SUCCESS) {
    return rc;
  } // open file for reading
  std::basic_fstream<char_type> ifs(m_fpath, openmode_r);
  // invariants & validation/safety checks
  if (!ifs) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_CTOR_ERRO);
    return rc::FS_CTOR_ERRO;
  }
  if (ifs.fail()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_OPEN_ERRO);
    return rc::FS_OPEN_ERRO;
  }
  if (!ifs.good()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_IO_UNAVAILABLE);
    return rc::FS_IO_UNAVAILABLE;
  }
  try { // we check on the ifs.fail() => will not be negative!
    auto const size{ ifs.tellg() };
    ifs.seekg(0, ifs.beg);
    ifs.read(m_block, size);
    WNDX_LOG(LL::INFO, "{} SUCCESS\n", fn);
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

[[nodiscard]] rc File::read_to_block()
{
  static constexpr auto fn{ "File::read_to_block()" };
  rc                    rc = heap_alloc();
  if (rc != rc::SUCCESS) {
    return rc;
  }
  try {
    rc = fcontent();
    if (rc != rc::SUCCESS) {
      WNDX_LOG(LL::ERRO, "{} -> {}\n", fn, rc);
      return rc;
    }
    WNDX_LOG(LL::DBUG, "{} SUCCESS : {}\n", fn, m_fpath.c_str());
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    File::~File();
    return rc::CRIT_EX_UNHANDLED;
  } catch (...) {
    WNDX_LOG(LL::CRIT, "{} occurred but was caught!\n{}\n",
             rc::CRIT_EX_ANONYMOUS, "THIS IS VERY BAD!");
    return rc::CRIT_EX_ANONYMOUS;
  }
  return rc::SUCCESS;
}

void File::print_fcontent() const noexcept
{
  if (m_block == nullptr) {
    return;
  }
#if MQLQD_FILE_CONTENTS_BOUNDARY
  std::cerr << ">>> [BEG] " << m_fpath << " - file content >>>" << '\n';
#endif // MQLQD_FILE_CONTENTS_BOUNDARY
  // NOTE: for loop is necessary for printing full contents of the binary file!
  for (size_t i = 0; i < m_block_size; ++i) {
    fmt::print("{:c}", m_block[i]); // NOLINT(*-pointer-arithmetic)
  }
#if MQLQD_FILE_CONTENTS_BOUNDARY
  std::cerr << "<<< [END] " << m_fpath << " - file content <<<" << '\n';
#endif // MQLQD_FILE_CONTENTS_BOUNDARY
}

} // namespace wndx::mqlqd::file
