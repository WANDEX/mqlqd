
#include "file.hpp"

#include "aliases.hpp" // sv_t & other project-wide aliases

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <system_error> // ec - error_code

namespace mqlqd {
namespace file {

namespace fs = std::filesystem;

inline constexpr auto openmode{ std::ios::in | std::ios::binary | std::ios::ate };

/**
 * @brief check that the file is regular file.
 *
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
int is_r(fs::path const& fpath) noexcept
{
  auto ec{ std::make_error_code(std::errc::operation_not_permitted) }; // XXX: mock
  fs::file_status s{ fs::status(fpath, ec) }; // for the noexcept
  // TODO: check file permissions (maybe it is a good idea to have)
  if (!fs::is_regular_file(s)) return 1;
  return 0;
}


/**
 * @brief read file contents into the memory block.
 *
 * @param  mem_block - memory block.
 * @param  fpath - file path to the file.
 * @return 0 on success or return code based on the failed check.
 */
int fcontent(char *mem_block, fs::path const& fpath)
{
  // TODO: use unique_ptr for the mem_block?
  // TODO: DOUBTS: use enum instead of rc?
  int rc{ is_r(fpath) }; // return code
  if (rc != 0) return rc;
  // open file for reading
  std::ifstream ifs(fpath, openmode);
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
    ifs.read(mem_block, size);

    std::cout << fpath << '\n'
              << "[INFO] ^ The entire contents of the file are in memory." << '\n';

  } catch(std::exception const& err) {
    std::cerr << "ERROR: an unhandled std::exception was caught:" << '\n'
              << err.what() << '\n' << __FILE__ << '\n';
    throw; // rethrow
  }
  return 0;
}

} // namespace file
} // namespace mqlqd

