// main client entry point

#include "aliases.hpp"
#include "config.hpp"

#include <cxxopts.hpp>

#include <algorithm>            // std::copy
#include <any>                  // std::any_cast()
#include <iostream>
#include <iterator>             // std::ostream_iterator
#include <string>               // std::to_string
#include <string_view>
#include <utility>              // std::forward
#include <vector>


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
}

int main (int argc, char *argv[])
{
  using namespace std::literals; // XXX

  using namespace mqlqd;

  // output every passed command line argument on stdout on separate lines.
  // std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, "\n")); // XXX

  cxxopts::Options options("mqlqd_client",
      "Transfer file(s) over TCP/IP to the server running the mqlqd_daemon.");
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
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (opts.count("file")) {
    // std::cout << opts["file"].as<cmd_opt_t>() << '\n';
    std::cout << opts["file"].as<sv_t>() << '\n';
  }

  return 0;
}

