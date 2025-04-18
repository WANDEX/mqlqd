// client command line (cmd)

#include "mqlqd/fclient.hpp"

#include "mqlqd/aliases.hpp"
#include "mqlqd/config.hpp"
#include "mqlqd/file.hpp"

#include <cxxopts.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>


namespace mqlqd {

// catch all possible exceptions (like Pokemons)
// to not suppress core dumps etc -> should be disabled => 0
#ifndef MQLQD_CATCH_THEM_ALL
#define MQLQD_CATCH_THEM_ALL 1
#endif // !MQLQD_CATCH_THEM_ALL

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
    options.custom_help("[OPTIONS]");
    options.positional_help("[file paths as trailing arguments ...]");
    options.set_width(80);
    options.add_options()
      ("a,addr", "Server IP address with the mqlqd_daemon. "
                 "(default: " + std::string{mqlqd::cfg::addr} + ')',
       cxxopts::value<cmd_opt_t>())

      ("p,port", "Port number of the daemon on the server. "
                 "(default: " + fmt::to_string<port_t>(mqlqd::cfg::port) + ')',
       cxxopts::value<port_t>())

      ("c,cat",  "Print file content (cat like utility mode).")
      ("f,file", "File path of the file to transmit.",
       cxxopts::value<std::vector<cmd_opt_t>>())

      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7")

      ("files_trail", "File path(s) as trailing argument(s).",
       cxxopts::value<std::vector<cmd_opt_t>>());
    // to support file paths supplied as trailing arguments:
    // e.g. (after all options) file1.txt file2.txt file3.txt
    options.parse_positional({"files_trail"});

    // initialize cmd options variable
    cxxopts::ParseResult cmd_opts{ options.parse(argc, argv) };

    // initialize logger with the specific log file.
    log_g  = Logger{ "/tmp/mqlqd/logs/client.log"sv };

    if (cmd_opts.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (!cmd_opts.count("file") && !cmd_opts.count("files_trail")) {
      std::cerr << options.help() << '\n'
                << "Look up the usage help." << '\n'
                << "No files were provided, exit." << '\n';
      exit(10);
    }

    if (cmd_opts.count("urge")) {
      // force specific log urgency level. (has priority over the value in config).
      const LL urgency{ cmd_opts["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }

    int rc{ 42 }; // reusable variable for the return codes

    // total number of file paths passed via all options: via option & positional arguments.
    const std::size_t n_files_passed{ cmd_opts.count("file") + cmd_opts.count("files_trail") };
    // vector of files info (subset of the File classes, helper info for the transmission)
    std::vector<file::mqlqd_finfo> vfinfo;
    vfinfo.reserve(n_files_passed);
    // vector of class instances
    std::vector<file::File> vfiles;
    vfiles.reserve(n_files_passed);

    if (cmd_opts.count("file")) {
      // add files passed via -f --file cmd options.
      for (fs::path const fp : cmd_opts["file"].as<std::vector<cmd_opt_t>>()) {
        vfiles.emplace_back(file::File{ fp, fs::file_size(fp) });
      }
    }
    if (cmd_opts.count("files_trail")) {
      // add files passed via trailing cmd arguments.
      for (fs::path const fp : cmd_opts["files_trail"].as<std::vector<cmd_opt_t>>()) {
        vfiles.emplace_back(file::File{ fp, fs::file_size(fp) });
      }
    }

    // loop over each file path passed via the cmd arguments (optional + positional).
    for (file::File &file : vfiles) {
      /**
       * Read contents of the file(s) into the block(s) of memory.
       * We are doing this here to not have potential bottleneck later -> on the transmission step.
       * (especially in terms of reading speed from the users block devices e.g. Slow HDD etc.).
       */
      rc = file.read_to_block();
      if (rc != 0) return rc;
      if (cmd_opts.count("cat")) {
        file.print_fcontent();
      } else {
        vfinfo.emplace_back(file.to_finfo());
      }
    }

    // if we are in the cat mode -> simply finish =>
    // as user do not need to initialize file client & do transmission.
    if (cmd_opts.count("cat")) return 0;

    // server address with the running mqlqd daemon. (file server)
    const addr_t addr{ cmd_opts.count("addr") ? cmd_opts["addr"].as<cmd_opt_t>() : mqlqd::cfg::addr };

    // port number of the daemon on the server. (cmd option overrides value from config)
    const port_t port{ cmd_opts.count("port") ? cmd_opts["port"].as<port_t>() : mqlqd::cfg::port };


    Fclient fclient{ addr, port };
    // initialize file client.
    rc = fclient.init();
    if (rc != 0) return rc;

    // attempt to send info of the upcoming transmission of the files.
    rc = fclient.send_files_info(vfinfo);
    if (rc != 0) return rc;

    // server is ready to accept provided files => start sending files.
    rc = fclient.send_files(vfiles);
    if (rc != 0) return rc;


  } catch(cxxopts::exceptions::exception const& err) {
    WNDX_LOG(LL::ERRO, "Fail during parsing of the cmd options:\n{}\n", err.what());
    return 1;
  } catch(std::exception const& err) {
    WNDX_LOG(LL::CRIT, "UNHANDLED std::exception was caught:\n{}\n", err.what());
    return 2;
#if MQLQD_CATCH_THEM_ALL
  } catch(...) {
    WNDX_LOG(LL::CRIT, "UNHANDLED anonymous exception occurred but was caught!\n{}\n", "THIS IS VERY BAD!");
    return 3;
#endif // MQLQD_CATCH_THEM_ALL
  }
  return 0;
}

} // namespace mqlqd

