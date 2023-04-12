// main client entry point

#include "aliases.hpp"
#include "config.hpp"

#include <cxxopts.hpp>

#include <algorithm>            // std::copy
#include <any>                  // std::any_cast()
#include <exception>
#include <iostream>
#include <iterator>             // std::ostream_iterator
#include <string>               // std::to_string
#include <string_view>
#include <utility>              // std::forward
#include <vector>

// catch all possible exceptions (like Pokemons)
// disabled by default -> to not suppress core dumps etc.
#ifndef CATCH_THEM_ALL
#define CATCH_THEM_ALL 0
#endif // !CATCH_THEM_ALL

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
 * @brief parse command line options.
 *
 * catches every possible exception & signifies about that:
 * with an error message printed to std::cerr.
 * and with the return code / exit code.
 *
 * @param  argc - as in the usual main().
 * @param  argv - as in the usual main().
 * @return error code.
 */
int cmd_opts(int argc, const char *argv[])
{
  using namespace std::literals; // XXX
  using namespace mqlqd;

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
      ("h,help", "Show usage help.")
      ("file_paths", "File path(s) as trailing argument(s).",
       cxxopts::value<std::vector<cmd_opt_t>>());
    // to support multiple files as trailing arguments: file1.txt file2.txt file3.txt
    // XXX: UNIMPLEMENTED
    options.parse_positional({"file_paths"});
    auto opts{ options.parse(argc, argv) };

    if (opts.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (opts.count("file")) {
      try {
        std::cout << opts["file"].as<cmd_opt_t>() << '\n';
      } catch(std::bad_cast const& err) {
        std::cerr << "ERROR: cxxopts opts as miscast - during parsing of the cmd options:" << '\n'
                  << err.what() << '\n';
        throw; // rethrow
      }
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

int main(int argc, const char *argv[])
{
  int return_code{ -1 }; // also known as the error code
  return_code = cmd_opts(argc, argv);
  if (return_code != 0) return return_code;
  // potential place for the extra steps (for the future build up)

  return 0;
}

