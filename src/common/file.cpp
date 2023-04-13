
#include "file.hpp"

#include "aliases.hpp"          // sv_t & other project-wide aliases

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <system_error>         // ec - error_code
#include <memory>               // std::unique_ptr


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
{}


[[nodiscard]]
int File::heap_alloc() noexcept
{
  // TODO: it is easy to use std::unique_ptr here for the mem_block
  // but maybe there is the benefit in proper using of the raw pointers?
  // It is easy to rewrite if requested!
  try {
    m_block = { new char[m_block_size] };
    if (!m_block) {
      std::cerr << "FILE CRITICAL ERROR: memory block == nullptr!" << '\n';
      return 30;
    }
  } catch(std::bad_alloc const& err) {
      std::cerr << "ERROR: std::bad_alloc - insufficient memory? :" << '\n'
                << err.what() << '\n';
      File::~File();
      return 31;
  } catch(std::exception const& err) {
      std::cerr << "ERROR: File::heap_alloc() std::exception suppressed:" << '\n'
                << err.what() << '\n';
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
    std::cerr << "ERROR: can not open input file!" << '\n';
    return 20;
  }
  if (ifs.fail()) {
    std::cerr << "ERROR: ifs.fail()" << '\n';
    return 21;
  }
  if (!ifs.good()) {
    std::cerr << "ERROR: !ifs.good()" << '\n';
    return 22;
  }

  try {
    // NOTE: we check on the ifs.fail() => will not be negative!
    const auto size{ ifs.tellg() };

    ifs.seekg(0, ifs.beg);
    ifs.read(m_block, size);

    std::cout << m_fpath << '\n'
              << "[INFO] ^ The entire contents of the file are in memory." << '\n';

  } catch(std::exception const& err) {
    std::cerr << "ERROR: an unhandled std::exception was caught:" << '\n'
              << err.what() << '\n' << __FILE__ << '\n';
    throw; // rethrow
  }
  return 0;
}

[[nodiscard]]
int File::read_to_block()
{
  int arc{ heap_alloc() };
  if (arc != 0) return arc;
  try {
    std::cout << "file path: " << m_fpath << '\n';
    int rrc { file::is_r(m_fpath) };
    std::cout << "rrc - is the regular file: " << std::boolalpha << (rrc == 0) << '\n';
    int crc { fcontent() };
    std::cout << "crc: " << crc << '\n';
  }
  catch(std::exception const& err) {
    std::cerr << "FILE CRITICAL ERROR: an unhandled std::exception occurred!" << '\n'
      << "THIS IS VERY BAD!" << '\n'
      << err.what() << '\n';
    return 5;
  } catch(...) {
    std::cerr << "FILE CRITICAL ERROR: an unhandled anonymous exception occurred!" << '\n'
      << "THIS IS EXTREMELY BAD!" << '\n';
    return 6;
  }
  return 0;
}

void File::print_fcontent() const
{
  if (!m_block) {
    return;
  }
  std::cout << ">>> [BEG] file content >>>" << '\n';
  for (std::size_t i = 0; i < m_block_size; i++) {
    std::cout << m_block[i];
  }
  std::cout << '\n';
  std::cout << "<<< [END] file content <<<" << '\n';
}

} // namespace file
} // namespace mqlqd

