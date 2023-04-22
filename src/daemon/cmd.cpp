// daemon command line (cmd)

#include "aliases.hpp"
#include "config.hpp"
#include "op.hpp"

#include "fserver.hpp"

#include <cxxopts.hpp>

#include <iostream>


namespace mqlqd {

// catch all possible exceptions (like Pokemons)
// to not suppress core dumps etc -> should be disabled => 0
#ifndef CATCH_THEM_ALL
#define CATCH_THEM_ALL 1
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
    cxxopts::Options options("mqlqd_daemon",
        "Accept file(s) over TCP/IP from the clients running the mqlqd_client.");
    options.set_width(80);
    options.add_options()
      ("p,port", "Use port number as identity of the daemon on the server.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_port)))
      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7");

    // initialize global options variable
    opts_g = options.parse(argc, argv);

    // initialize logger with the specific log file.
    log_g  = Logger{ "/tmp/mqlqd/logs/daemon.log"sv };

    if (opts_g.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (opts_g.count("urge")) {
      // force specific log urgency level.
      // (has priority over the value in config).
      LL urgency{ opts_g["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }


    Fserver fserver;
    // initialize file server.
    int fs_rc = fserver.init();
    if (fs_rc != 0) {
      log_g.msg(LL::ERRO, fmt::format("fserver.init() -> {}", fs_rc));
      return fs_rc;
    }
    // TODO: recv number of files to receive, their file names etc.
    //       before starting receiving files.
    // TODO: recv files
    // TODO: rewrite with the new proper logic like in the file client


    // XXX: obsolete rewrite or delete me!
    /*
    int rf_rc{ -1 };
    int recv_files_count{ 0 };
    rf_rc = fserver.recv_file();
    if (rf_rc != 0) {
      log_g.msg(LL::ERRO, fmt::format("Fail fserver.recv_file() -> {}", rf_rc));
    } else {
      ++recv_files_count;
      // log_g.msg(LL::STAT, fmt::format("Successfully recv file: {}", f.m_fpath.string()));
      log_g.msg(LL::STAT, fmt::format("Successfully recv file: {}", "UNIMP: filename")); // TODO filename
    }
    log_g.msg(LL::NTFY, fmt::format("[{}/{}] files were successfully recv. UNIMP: N files!",
              recv_files_count, "1")); // FIXME: hardcoded 1
    */

  } catch(cxxopts::exceptions::exception const& err) {
    log_g.msg(LL::ERRO, fmt::format("Fail during parsing of the cmd options:\n{}\n", err.what()));
    return 1;
  } catch(std::exception const& err) {
    log_g.msg(LL::CRIT, fmt::format("Unhandled std::exception was caught:\n{}\n", err.what()));
    return 2;
#if CATCH_THEM_ALL
  } catch(...) {
    log_g.msg(LL::CRIT, "Unhandled anonymous exception occurred but was caught!\nTHIS IS VERY BAD!\n");
    return 3;
#endif // CATCH_THEM_ALL
  }
  return 0;
}

} // namespace mqlqd

