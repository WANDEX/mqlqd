// main client entry point

#include "aliases.hpp"
#include "config.hpp"
#include "file.hpp"

#include <cxxopts.hpp>

// TODO: clean unused includes
#include <algorithm>            // std::copy
#include <any>                  // std::any_cast()
#include <exception>
#include <filesystem>
#include <iostream>
#include <iterator>             // std::ostream_iterator
#include <source_location>      // std::source_location
#include <string>               // std::to_string
#include <string_view>
#include <utility>              // std::forward
#include <vector>

namespace mqlqd {

namespace fs = std::filesystem;

// catch all possible exceptions (like Pokemons)
// disabled by default -> to not suppress core dumps etc.
#ifndef CATCH_THEM_ALL
#define CATCH_THEM_ALL 0
#endif // !CATCH_THEM_ALL

// global for simplicity - no drawbacks => be able to use it in functions ->
// without the need to pass it into every single function as the extra argument.
// (usually cmd options are needed for the whole life of the program anyway!)
static cxxopts::ParseResult opts_g;

/*
inline constexpr std::string
dvw(auto &&value) noexcept
{
  // NOTE: std::to_string() does not work for all types!
  return std::any_cast<std::string>(value);
}
*/

/**
 * @brief default value wrapper for the cxxopts.
 *
 * It is needed because default_value accepts std::string.
 * Convert std::string_view -> std::string.
 *
 * @param  input value
 * @return output value as std::string type
 */
inline constexpr std::string
dvw(std::string_view const& value) noexcept
{
  return static_cast<std::string>(value);
  // XXX: which is better?
  // return std::string{ value };
}


/**
 * @brief cxxopts option wrapper to unify & simplify exceptions handling.
 */
[[nodiscard]] constexpr cmd_opt_t
opt_wrap(std::string_view const& in_opt)
{
  try {
    return opts_g[dvw(in_opt)].as<cmd_opt_t>();
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
 */
[[nodiscard]] fs::path
opt_file_wrap(std::string_view const& in_opt)
{
  try {
    return fs::path{ opt_wrap(in_opt) };
  } catch(std::exception const& err) {
    std::cerr << "ERROR: THIS WAS UNEXPECTED! an unhandled std::exception!" << '\n'
              << "in '" << __FILE__ << "' client.cpp opt_file_wrap():" << '\n'
              << err.what() << '\n';
    throw; // rethrow
  }
}

/**
 * @brief parse command line options.
 *
 * catches every possible exception & signifies about that:
 * with an error message printed to std::cerr.
 * and with the return code / exit code.
 *
 * @param  argc - as in the usual main() entry point.
 * @param  argv - as in the usual main() entry point.
 * @return error code.
 */
int cmd_opts(int argc, const char *argv[])
{
  try
  {
    cxxopts::Options options("mqlqd_client",
        "Transfer file(s) over TCP/IP to the server running the mqlqd_daemon.");
    options.set_width(80);
    options.add_options()
      ("a,addr", "Server IP address with the mqlqd_daemon.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_addr)))

      ("p,port", "Port number of the daemon on the server.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_port)))

      ("f,file", "File path of the file to transmit.", cxxopts::value<cmd_opt_t>())
      ("h,help", "Show usage help.");
    /*
     *   ("file_paths", "File path(s) as trailing argument(s).",
     *    cxxopts::value<std::vector<cmd_opt_t>>());
     * // to support multiple files as trailing arguments: file1.txt file2.txt file3.txt
     * options.parse_positional({"file_paths"});
     * // XXX: UNIMPLEMENTED ^
     */

    opts_g = options.parse(argc, argv); // initialize global options variable

    if (opts_g.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (!opts_g.count("file") && !opts_g.count("msg")) { // XXX: msg UNIMPLEMENTED
        std::cerr << options.help() << '\n';
        std::cerr << "Look up the usage help." << '\n'
                  << "No files were provided, exit." << '\n';
        exit(10);
    }

    if (opts_g.count("file")) {
      const fs::path fp{ opt_file_wrap("file") };
      file::File file{ fp, fs::file_size(fp) };
      auto rc{ file.read_to_block() };
      std::cout << rc << " : rc file.read_to_block()" << '\n';
      file.print_fcontent();
    }

  } catch(cxxopts::exceptions::exception const& err) {
    std::cerr << "ERROR: during parsing of the cmd options:" << '\n'
              << err.what() << '\n';
    return 1;
#if CATCH_THEM_ALL
  } catch(std::exception const& err) {
    std::cerr << "CRITICAL ERROR: an unhandled std::exception was caught:" << '\n'
              << err.what() << '\n';
    return 2;
  } catch(...) {
    std::cerr << "CRITICAL ERROR: an unhandled anonymous exception occurred!" << '\n'
              << "THIS IS VERY BAD!" << '\n';
    return 3;
#endif // CATCH_THEM_ALL
  }
  return 0;
}

} // namespace mqlqd

int main(int argc, const char *argv[])
{
  int return_code{ -1 }; // also known as the error code
  return_code = mqlqd::cmd_opts(argc, argv);
  if (return_code != 0) return return_code;
  // potential place for the extra steps (for the future build up)

  return 0;
}

