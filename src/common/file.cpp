
#include "file.hpp"

#include "aliases.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <system_error>         // ec - error_code
#include <memory>               // std::unique_ptr


// show boundaries of the files in the cat mode
#ifndef FILE_CONTENTS_BOUNDARY
#define FILE_CONTENTS_BOUNDARY 0
#endif // !FILE_CONTENTS_BOUNDARY

namespace mqlqd {
namespace file {

/**
 * @brief check that the file is regular file.
 *
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
[[nodiscard]]
int is_r(fs::path const& fpath) noexcept
{
  auto ec{ std::make_error_code(std::errc::operation_not_permitted) }; // XXX: mock
  fs::file_status s{ fs::status(fpath, ec) }; // for the noexcept
  // TODO: check file permissions (maybe it is a good idea to have)
  if (!fs::is_regular_file(s)) return 1;
  return 0;
}


File::File(fs::path const& fpath, const std::size_t sz) noexcept
  : m_fpath{ fpath }, m_block_size{ sz }
{
  log_g.msg(LL::DBUG, "ctor - File instance.");
}


[[nodiscard]]
int File::heap_alloc() noexcept
{
  // TODO: it is easy to use std::unique_ptr here for the mem_block
  // but maybe there is the benefit in proper using of the raw pointers?
  // It is easy to rewrite if requested!
  try {
    m_block = { new char[m_block_size] };
    if (!m_block) {
      log_g.msg(LL::CRIT, "File::heap_alloc() memory block == nullptr!");
      return 30;
    }
  } catch(std::bad_alloc const& err) {
      log_g.msg(LL::ERRO, fmt::format("std::bad_alloc - insufficient memory? :\n{}\n", err.what()));
      File::~File();
      return 31;
  } catch(std::exception const& err) {
      log_g.msg(LL::CRIT, fmt::format("File::heap_alloc() unhandled std::exception suppressed:\n{}\n", err.what()));
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
  std::ifstream ifs(m_fpath, openmode);
  // invariants & validation/safety checks
  if (!ifs) {
    log_g.msg(LL::ERRO, "can not open input file!");
    return 20;
  }
  if (ifs.fail()) {
    log_g.msg(LL::ERRO, "ifs.fail()");
    return 21;
  }
  if (!ifs.good()) {
    log_g.msg(LL::ERRO, "!ifs.good()");
    return 22;
  }

  try {
    // NOTE: we check on the ifs.fail() => will not be negative!
    const auto size{ ifs.tellg() };

    ifs.seekg(0, ifs.beg);
    ifs.read(m_block, size);

    log_g.msg(LL::DBUG, "The entire contents of the file are in memory.");

  } catch(std::exception const& err) {
    log_g.msg(LL::CRIT, fmt::format("File::fcontent() unhandled std::exception suppressed:\n{}\n", err.what()));

    // TODO: make this possible.
    // log_g.msg(LL::CRIT) << "an unhandled std::exception was caught:" << '\n';
    // log_g.msg << LL::CRIT << "an unhandled std::exception was caught:" << '\n';
    //       << err.what() << '\n' << __FILE__ << '\n';

    return 25;
  }
  return 0;
}

[[nodiscard]]
int File::read_to_block()
{
  int arc{ heap_alloc() };
  if (arc != 0) return arc;
  try {
    // XXX: this is not very convenient & certainly not terse.
    log_g.msg(LL::DBUG, "file path: {}\n", fmt::make_format_args(m_fpath.string()));
    int rrc { file::is_r(m_fpath) };
    // TODO: make this work. (hard)
    // log_g.msg(LL::INFO, "rrc - is the regular file: {}\n", (rrc == 0));
    log_g.msg(LL::DBUG, fmt::format("rrc - is the regular file: {}", (rrc == 0)));
    int crc { fcontent() };
    log_g.msg(LL::DBUG, fmt::format("crc: {}", crc));
  }
  catch(std::exception const& err) {
    log_g.msg(LL::CRIT, fmt::format("File::read_to_block() unhandled std::exception suppressed:\n{}\n", err.what()));
    return 5;
  } catch(...) {
    log_g.msg(LL::CRIT, "File::read_to_block() unhandled anonymous exception occurred but was caught!\nTHIS IS EXTREMELY BAD!\n");
    return 6;
  }
  return 0;
}

void File::print_fcontent() const
{
  if (!m_block) {
    return;
  }
#if FILE_CONTENTS_BOUNDARY
  std::cerr << ">>> [BEG] " << m_fpath << " - file content >>>" << '\n';
#endif // FILE_CONTENTS_BOUNDARY
  for (std::size_t i = 0; i < m_block_size; i++) {
    std::cout << m_block[i];
  }
#if FILE_CONTENTS_BOUNDARY
  std::cerr << "<<< [END] " << m_fpath << " - file content <<<" << '\n';
#endif // FILE_CONTENTS_BOUNDARY
}

} // namespace file
} // namespace mqlqd

