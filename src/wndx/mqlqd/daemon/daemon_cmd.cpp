// daemon command line (cmd)

#include "wndx/mqlqd/aliases.hpp"

#include "wndx/mqlqd/fserver.hpp"

#include "wndx/mqlqd/config.hpp"
#include "wndx/mqlqd/file.hpp"
#include "wndx/mqlqd/rc.hpp"

#include <cxxopts.hpp>

#include <filesystem>
#include <iostream>
#include <string>


// catch all possible exceptions (like Pokemon's)
// to not suppress core dumps etc -> should be disabled => 0
// clang-format off
#ifndef MQLQD_CATCH_THEM_ALL
#define MQLQD_CATCH_THEM_ALL 1 // NOLINT(*-macro-usage)
#endif//MQLQD_CATCH_THEM_ALL
// clang-format on


namespace wndx::mqlqd {

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
// NOLINTNEXTLINE(*-avoid-c-arrays)
[[nodiscard]] rc cmd_opts(int argc, char const* argv[])
{
  try {
    // clang-format off
    cxxopts::Options options("mqlqd_daemon",
        "Accept file(s) over TCP/IP from the clients running the mqlqd_client.");
    options.set_width(80); // NOLINT(*-magic-numbers) - standard TERM width
    options.add_options()
      ("d,dir", "Path to the storage dir, else default storage under cwd.",
       cxxopts::value<cmd_opt_t>()->default_value("./mqlqd_storage"))

      ("p,port", "Use port number as identity of the daemon on the server. "
                 "(default: " + fmt::to_string<port_t>(mqlqd::cfg::port) + ')',
       cxxopts::value<port_t>())

      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7");
    // clang-format on
    // initialize cmd options variable
    cxxopts::ParseResult cmd_opts{ options.parse(argc, argv) };

    // initialize logger with the specific log file.
    log_g = Logger{ "/tmp/mqlqd/logs/daemon.log"sv };

    if (cmd_opts.count("help")) {
      std::cout << options.help() << '\n';
      return rc::SUCCESS;
    }

    if (cmd_opts.count("urge")) { // force specific log urgency level
      const LL urgency{ cmd_opts["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }

    rc rc{ rc::INIT }; // reusable variable for the return codes

    // path to the storage dir. (storage for incoming files)
    fs::path const storage_dir{ cmd_opts["dir"].as<cmd_opt_t>() };

    std::error_code ec{};
    if (fs::is_directory(storage_dir, ec)) {
      WNDX_LOG(LL::NTFY,
               "Directory exist, and will be used as the storage dir\n");
    } else {
      // make dir for the storage with permissions for owner only.
      rc = file::mkdir(storage_dir, fs::perms::owner_all);
      if (rc != rc::SUCCESS) {
        return rc;
      }
    }

    // use port number as identity on the server. (cmd option overrides value
    // from config)
    port_t const port{ cmd_opts.count("port") ? cmd_opts["port"].as<port_t>()
                                              : mqlqd::cfg::port };

    /**
     * Work infinitely as the daemon till one of the stop signals received.
     * Also - till the error: return code, errno msg, everything is logged,
     * nothing suppressed.)
     * TODO: make this daemon (file server) loop more optimal.
     */
    for (;;) {
      Fserver fserver{ port, storage_dir };
      // initialize file server.
      rc = fserver.init();
      if (rc != rc::SUCCESS) {
        return rc;
      }

      // attempt to receive info of the upcoming transmission of the files.
      rc = fserver.recv_files_info();
      if (rc != rc::SUCCESS) {
        return rc;
      }

      // server is ready to accept provided files => start accepting files.
      rc = fserver.recv_files();
      if (rc != rc::SUCCESS) {
        return rc;
      }
    }

  } catch (cxxopts::exceptions::exception const& err) {
    WNDX_LOG(LL::ERRO, "{}:\n{}\n", rc::ERRO_CMD_OPT, err.what());
    return rc::ERRO_CMD_OPT;
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} was caught:\n{}\n", rc::CRIT_EX_UNHANDLED,
             err.what());
    return rc::CRIT_EX_UNHANDLED;
#if MQLQD_CATCH_THEM_ALL
  } catch (...) {
    WNDX_LOG(LL::CRIT, "{} occurred but was caught!\n{}\n",
             rc::CRIT_EX_ANONYMOUS, "THIS IS VERY BAD!");
    return rc::CRIT_EX_ANONYMOUS;
#endif // MQLQD_CATCH_THEM_ALL
  }
  return rc::SUCCESS;
}

} // namespace wndx::mqlqd
