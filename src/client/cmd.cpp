// client command line (cmd)

#include "aliases.hpp"
#include "config.hpp"
#include "file.hpp"
#include "op.hpp"

#include <cxxopts.hpp>

#include <iostream>


namespace mqlqd {

// catch all possible exceptions (like Pokemons)
// disabled by default -> to not suppress core dumps etc.
#ifndef CATCH_THEM_ALL
#define CATCH_THEM_ALL 0
#endif // !CATCH_THEM_ALL

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
[[nodiscard]] int
cmd_opts(int argc, const char *argv[])
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

