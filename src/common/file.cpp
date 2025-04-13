
#include "mqlqd/aliases.hpp"

#include "mqlqd/file.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <system_error>         // ec - error_code
#include <memory>               // std::unique_ptr


// show boundaries of the files in the cat mode
#ifndef MQLQD_FILE_CONTENTS_BOUNDARY
#define MQLQD_FILE_CONTENTS_BOUNDARY 0
#endif // !MQLQD_FILE_CONTENTS_BOUNDARY

namespace mqlqd {
using namespace wndx;

namespace file {

/**
 * @brief check that the file is regular file.
 *
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
[[nodiscard]] int
is_r(fs::path const& fpath) noexcept
{
  std::error_code ec {};
  fs::file_status s{ fs::status(fpath, ec) }; // for the noexcept
  if (ec) {
    WNDX_LOG(LL::CRIT, "{}\n^ fs::status error:\n{}\n",
                       fpath.c_str(), ec.message() );
    return ec.value();
  }
  // TODO: check file permissions (maybe it is a good idea to have)
  if (!fs::is_regular_file(s)) {
    WNDX_LOG(LL::ERRO, "IS NOT the regular file: {}\n", fpath.c_str());
    return 1;
  }
  WNDX_LOG(LL::INFO, "Is the regular file: {}\n", fpath.c_str());
  return 0;
}


/**
 * @brief make dir and set required perms. (should be safe)
 *
 * @param  new dir path.
 * @param  new dir permissions.
 * @param  force/insist on importance of the access rights to the dir.
 * @return 0 on success, else non zero return code.
 */
[[nodiscard]] int
mkdir(fs::path const& dpath, fs::perms const& perms, bool force) noexcept
{
  std::error_code ec {};
  if (fs::is_directory(dpath, ec)) {
    WNDX_LOG(LL::DBUG, "Directory exist, and will be used as is\n", "");
    if (force) {
      // TODO: validate that directory permissions are set as requested.
    }
  } else {
    if (!fs::exists(dpath, ec)) {
      if (fs::create_directory(dpath, dpath.parent_path(), ec)) {
        fs::permissions(dpath, perms, ec);
        if (ec.value() == 0) {
          WNDX_LOG(LL::NTFY, "Created new directory with requested access permissions.\n", "");
        } else {
          WNDX_LOG(LL::ERRO, "Created new directory, but failed to set requested perms.\n", "");
          if (fs::remove(dpath, ec)) {
            WNDX_LOG(LL::NTFY, "Removed empty directory created with incorrect perms.\n", "");
            return 7;
          } else {
            WNDX_LOG(LL::CRIT,
                      "{}\n^ failed to remove this dir\n"
                      "that was created with incorrect access rights.\n"
                      "Because of the perms, do it yourself manually!\n"
                      "Additionally take a look at the origin of the error:\n{}\n",
                      dpath.c_str(), ec.message() );
            return ec.value();
          }
        }
      } else {
        WNDX_LOG(LL::CRIT,
                  "If your intent was to make this dir under current cwd:\n"
                  "{}\n^ precede your path with './' i.e. ./{}\n"
                  "Additionally take a look at the origin of the error:\n"
                  "fs::create_directory error: {}\n",
                  dpath.c_str(), dpath.c_str(), ec.message() );
        return ec.value();
      }
    } else {
      auto realpath{ fs::canonical(dpath, ec) };
      if (ec) {
        WNDX_LOG(LL::CRIT, "{}\n^ canonical path error:\n{}\n",
                            dpath.c_str(), ec.message() );
        return ec.value();
      }
      WNDX_LOG(LL::CRIT, "{}\n^ path cannot be used as the new directory!\n",
                          realpath.c_str() );
      return 6;
    }
  }
  return 0;
}


File::File(fs::path const& fpath, const std::size_t sz) noexcept
  : m_fpath{ fpath }, m_block_size{ sz }
{
  WNDX_LOG(LL::DBUG, "ctor - File instance from file & size:\n"
                     "\t[{}] {}\n", m_block_size, m_fpath.c_str() );
}

File::File(mqlqd_finfo const& finfo, fs::path dpath) noexcept
  : m_block_size{ finfo.block_size }
{
  // concatenate file name from file info to the passed directory path.
  // NOTE: like this -> additional directory separators are never introduced.
  dpath += '/';
  dpath += finfo.fname;
  m_fpath = dpath;
  WNDX_LOG(LL::DBUG, "ctor - File instance from finfo:\n"
                     "\t[{}] {}\n", m_block_size, m_fpath.c_str() );
}

[[nodiscard]] mqlqd_finfo
File::to_finfo() const noexcept
{
  mqlqd_finfo finfo {};
  finfo.block_size = m_block_size;
  // fill the array - file name (element by element)
  auto fname{ m_fpath.filename().string() };
  for (size_t i = 0; i < fname.length(); i++) {
    finfo.fname[i] = fname[i];
  }
  // null terminator after the file name (important)
  finfo.fname[fname.length()] = '\0';
  return finfo;
}

[[nodiscard]] int
File::write()
{
  if (!m_block) {
    WNDX_LOG(LL::CRIT, "empty memory block, nothing to write!\n", "");
    return 33;
  }
  // open file for writing
  std::basic_fstream<char_type> ofs(m_fpath, openmode_w);
  // TODO: reinspect checks
  if (!ofs) {
    WNDX_LOG(LL::ERRO, "can not open output file!\n", "");
    return 30;
  }
  if (ofs.fail()) {
    WNDX_LOG(LL::ERRO, "ofs.fail()\n", "");
    return 31;
  }
  if (!ofs.good()) {
    WNDX_LOG(LL::ERRO, "!ofs.good()\n", "");
    return 32;
  }

  try {
    // NOTE: for loop is necessary for writing full contents of the binary file!
    for (size_t i = 0; i < m_block_size; i++) {
      ofs << m_block[i];
    }
    WNDX_LOG(LL::STAT, "Contents of the memory block were written to the file:\n\t{}\n", m_fpath.c_str());
  } catch(std::exception const& err) {
    WNDX_LOG(LL::CRIT, "File::write() UNHANDLED std::exception suppressed:\n{}\n", err.what());
    return 35;
  }
  return 0;
}

[[nodiscard]]
int File::heap_alloc() noexcept
{
  // TODO: it is easy to use std::unique_ptr here for the mem_block
  // but maybe there is the benefit in proper using of the raw pointers?
  // It is easy to rewrite if requested!
  try {
    m_block = { new char_type[m_block_size]{} };
    if (!m_block) {
      WNDX_LOG(LL::CRIT, "[FAIL] File::heap_alloc() memory block == nullptr!\n", "");
      return 30;
    }
    WNDX_LOG(LL::DBUG, "[ OK ] File::heap_alloc()\n", "");
  } catch(std::bad_alloc const& err) {
      WNDX_LOG(LL::ERRO, "std::bad_alloc - insufficient memory? :\n{}\n", err.what());
      File::~File();
      return 31;
  } catch(std::exception const& err) {
      WNDX_LOG(LL::CRIT, "File::heap_alloc() UNHANDLED std::exception suppressed:\n{}\n", err.what());
      File::~File();
      return 32;
  }
  return 0;
}

void File::heap_cleanup() noexcept
{
  delete[] m_block;
  m_block = nullptr;
}

File::~File() noexcept
{
  heap_cleanup();
}

/**
 * @brief read file contents into the memory block.
 *
 * @param  mem_block - memory block.
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
[[nodiscard]]
int File::fcontent()
{
  int rc{ is_r(m_fpath) }; // return code
  if (rc != 0) return rc;
  // open file for reading
  std::basic_fstream<char_type> ifs(m_fpath, openmode_r);
  // invariants & validation/safety checks
  if (!ifs) {
    WNDX_LOG(LL::ERRO, "can not open input file!\n", "");
    return 20;
  }
  if (ifs.fail()) {
    WNDX_LOG(LL::ERRO, "ifs.fail()\n", "");
    return 21;
  }
  if (!ifs.good()) {
    WNDX_LOG(LL::ERRO, "!ifs.good()\n", "");
    return 22;
  }

  try {
    // NOTE: we check on the ifs.fail() => will not be negative!
    const auto size{ ifs.tellg() };

    ifs.seekg(0, ifs.beg);
    ifs.read(m_block, size);

    WNDX_LOG(LL::INFO, "The entire contents of the file are in memory\n", "");

  } catch(std::exception const& err) {
    WNDX_LOG(LL::CRIT, "File::fcontent() UNHANDLED std::exception suppressed:\n{}\n", err.what());
    return 25;
  }
  return 0;
}

[[nodiscard]]
int File::read_to_block()
{
  int rc = heap_alloc();
  if (rc != 0) return rc;
  try {
    rc = fcontent();
    if (rc != 0) {
      WNDX_LOG(LL::ERRO, "[FAIL] File::read_to_block() -> {}\n", rc);
      return rc;
    }
    WNDX_LOG(LL::DBUG, "[ OK ] File::read_to_block() : {}\n", m_fpath.c_str());
  }
  catch(std::exception const& err) {
    WNDX_LOG(LL::CRIT, "File::read_to_block() UNHANDLED std::exception suppressed:\n"
                       "{}\n", err.what());
    return 5;
  } catch(...) {
    WNDX_LOG(LL::CRIT, "File::read_to_block() {}\n{}\n",
                       "UNHANDLED anonymous exception occurred but was caught!",
                       "THIS IS EXTREMELY BAD!");
    return 6;
  }
  return 0;
}

void File::print_fcontent() const noexcept
{
  if (!m_block) {
    return;
  }
#if MQLQD_FILE_CONTENTS_BOUNDARY
  std::cerr << ">>> [BEG] " << m_fpath << " - file content >>>" << '\n';
#endif // MQLQD_FILE_CONTENTS_BOUNDARY
  // NOTE: for loop is necessary for printing full contents of the binary file!
  for (size_t i = 0; i < m_block_size; i++) {
    fmt::print("{:c}", m_block[i]);
  }
#if MQLQD_FILE_CONTENTS_BOUNDARY
  std::cerr << "<<< [END] " << m_fpath << " - file content <<<" << '\n';
#endif // MQLQD_FILE_CONTENTS_BOUNDARY
}

} // namespace file
} // namespace mqlqd

