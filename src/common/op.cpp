// cxxopts & options related code

#include "op.hpp"

#include "aliases.hpp"

#include <cxxopts.hpp>

#include <exception>
#include <filesystem>
#include <iostream>
#include <source_location>      // TODO: make use of it std::source_location
#include <string>
#include <string_view>


namespace mqlqd {

/**
 * @brief default value wrapper for the cxxopts.
 *
 * It is needed because default_value accepts std::string.
 * Convert std::string_view -> std::string.
 *
 * @param  input value
 * @return output value as std::string type
 */
[[nodiscard]] cmd_opt_t
dvw(std::string_view const& value) noexcept
{
  return static_cast<cmd_opt_t>(value);
}


/**
 * @brief cxxopts option wrapper to unify & simplify exceptions handling.
 */
[[nodiscard]] cmd_opt_t
opt_wrap(std::string_view const& in_opt)
{
  try {
    return opts_g[static_cast<cmd_opt_t>(in_opt)].as<cmd_opt_t>();
  } catch(std::bad_cast const& err) {
    std::cerr << "ERROR: cxxopts - option as miscast:" << '\n'
              << err.what() << '\n';
    throw; // rethrow
  } catch(cxxopts::exceptions::exception const& err) {
    std::cerr << "ERROR: cxxopts - option cxxopts::exceptions::exception:" << '\n'
              << err.what() << '\n';
    throw; // rethrow
  } catch(std::exception const& err) {
    std::cerr << "ERROR: THIS WAS UNEXPECTED! an unhandled std::exception!" << '\n'
              << "in '" << __FILE__ << "' client.cpp opt_wrap():" << '\n'
              << err.what() << '\n';
    // std::clog << "ERROR: THIS WAS UNEXPECTED! an unhandled std::exception!" << '\n'
              // << "in '" << (std::source_location::current()) << "' client.cpp opt_wrap():" << '\n'
              // << err.what() << '\n';
    throw; // rethrow
  }
}

/**
 * @brief cxxopts option wrapper for the files to unify & simplify exceptions handling.
 *
 * @return fs::path type from the std::string_view/string.
 */
[[nodiscard]] fs::path
opt_file_wrap(std::string_view const& in_opt)
{
  try {
    // return fs::path{ opt_wrap(in_opt) };
    // return static_cast<fs::path>(in_opt);
    return in_opt;
  } catch(std::exception const& err) {
    std::cerr << "ERROR: THIS WAS UNEXPECTED! an unhandled std::exception!" << '\n'
              << "in '" << __FILE__ << "' client.cpp opt_file_wrap():" << '\n'
              << err.what() << '\n';
    throw; // rethrow
  }
}

} // namespace mqlqd

