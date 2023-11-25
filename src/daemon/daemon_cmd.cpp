// daemon command line (cmd)

#include "aliases.hpp"
#include "config.hpp"
#include "file.hpp"

#include "fserver.hpp"

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
    cxxopts::Options options("mqlqd_daemon",
        "Accept file(s) over TCP/IP from the clients running the mqlqd_client.");
    options.set_width(80);
    options.add_options()
      ("d,dir", "Path to the storage dir, else default storage under cwd.",
       cxxopts::value<cmd_opt_t>()->default_value("./mqlqd_storage"))

      ("p,port", "Use port number as identity of the daemon on the server. "
                 "(default: " + fmt::to_string<port_t>(cfg::port) + ')',
       cxxopts::value<port_t>())

      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7");

    // initialize cmd options variable
    cxxopts::ParseResult cmd_opts{ options.parse(argc, argv) };

    // initialize logger with the specific log file.
    log_g  = Logger{ "/tmp/mqlqd/logs/daemon.log"sv };

    if (cmd_opts.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (cmd_opts.count("urge")) {
      // force specific log urgency level. (has priority over the value in config).
      const LL urgency{ cmd_opts["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }

    int rc{ 42 }; // reusable variable for the return codes

    // path to the storage dir. (storage for incoming files)
    const fs::path storage_dir{ cmd_opts["dir"].as<cmd_opt_t>() };

    std::error_code ec {};
    if (fs::is_directory(storage_dir, ec)) {
      WNDX_LOG(LL::NTFY, "Directory exist, and will be used as the storage dir\n", "");
    } else {
      // make dir for the storage with permissions for owner only.
      rc = file::mkdir(storage_dir, fs::perms::owner_all);
      if (rc != 0) return rc;
    }

    // use port number as identity on the server. (cmd option overrides value from config)
    const port_t port{ cmd_opts.count("port") ? cmd_opts["port"].as<port_t>() : cfg::port };


    /**
     * Work infinitely as the daemon till one of the stop signals received.
     * Also - till the error: return code, errno msg, everything is logged, nothing suppressed.)
     * TODO: make this daemon (file server) loop more optimal.
     */
    for(;;)
    {
      Fserver fserver{ port, storage_dir };
      // initialize file server.
      rc = fserver.init();
      if (rc != 0) return rc;

      // attempt to receive info of the upcoming transmission of the files.
      rc = fserver.recv_files_info();
      if (rc != 0) return rc;

      // server is ready to accept provided files => start accepting files.
      rc = fserver.recv_files();
      if (rc != 0) return rc;
    }


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

