// client command line (cmd)

#include "wndx/mqlqd/aliases.hpp"

#include "wndx/mqlqd/fclient.hpp"

#include "wndx/mqlqd/config.hpp"
#include "wndx/mqlqd/file.hpp"
#include "wndx/mqlqd/rc.hpp"

#include <cxxopts.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>


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
    cxxopts::Options options("mqlqd_client",
      "Transfer file(s) over TCP/IP to the server running the mqlqd_daemon.");
    options.custom_help("[OPTIONS]");
    options.positional_help("[file paths as trailing arguments ...]");
    options.set_width(80); // NOLINT(*-magic-numbers) - standard TERM width
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
    // clang-format on
    // to support file paths supplied as trailing arguments:
    // e.g. (after all options) file1.txt file2.txt file3.txt
    options.parse_positional({ "files_trail" });

    // initialize cmd options variable
    cxxopts::ParseResult cmd_opts{ options.parse(argc, argv) };

    // initialize logger with the specific log file.
    log_g = Logger{ "/tmp/mqlqd/logs/client.log"sv };

    if (cmd_opts.count("help")) {
      std::cout << options.help() << '\n';
      return rc::SUCCESS;
    }

    if (!cmd_opts.count("file") && !cmd_opts.count("files_trail")) {
      WNDX_LOG(LL::WARN, "Lookup the usage via --help.\n{}, exit.\n",
               rc::WARN_CMD_FILE_REQ);
      return rc::WARN_CMD_FILE_REQ;
    }

    if (cmd_opts.count("urge")) { // force specific log urgency level
      const LL urgency{ cmd_opts["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }

    rc rc{ rc::INIT }; // reusable variable for the return codes

    // total number of file paths passed via the cmd args (opts + trailing)
    std::size_t const n_files_passed{ cmd_opts.count("file") +
                                      cmd_opts.count("files_trail") };
    // vector of files info - subset of the File classes,
    // helper info for the transmission
    std::vector<file::Finfo> vfinfo;
    vfinfo.reserve(n_files_passed);
    // vector of class instances
    std::vector<file::File> vfiles;
    vfiles.reserve(n_files_passed);

    if (cmd_opts.count("file")) { // add files via -f --file cmd options
      for (fs::path const fp : cmd_opts["file"].as<std::vector<cmd_opt_t>>()) {
        vfiles.emplace_back(fp, fs::file_size(fp));
      }
    }
    if (cmd_opts.count("files_trail")) { // add files via trailing cmd args
      for (fs::path const fp :
           cmd_opts["files_trail"].as<std::vector<cmd_opt_t>>())
      {
        vfiles.emplace_back(fp, fs::file_size(fp));
      }
    }

    // loop over each file path passed via the cmd args (opts + trailing)
    for (file::File& file : vfiles) {
      /**
       * Read contents of the file(s) into the block(s) of memory.
       * We are doing this here to not have potential bottleneck later -> on the
       * transmission step. (especially in terms of reading speed from the users
       * block devices e.g. Slow HDD etc.).
       */
      rc = file.read_to_block();
      if (rc != rc::SUCCESS) {
        return rc;
      }
      if (cmd_opts.count("cat")) {
        file.print_fcontent();
      } else {
        vfinfo.emplace_back(file.to_finfo());
      }
    }

    // if we are in the cat mode -> simply finish =>
    // as user do not need to initialize file client & do transmission.
    if (cmd_opts.count("cat")) {
      return rc::SUCCESS;
    }

    // server address with the running mqlqd daemon. (file server)
    addr_t const addr{ cmd_opts.count("addr") ? cmd_opts["addr"].as<cmd_opt_t>()
                                              : mqlqd::cfg::addr };

    // port number of the daemon on the server. (daemon instance)
    port_t const port{ cmd_opts.count("port") ? cmd_opts["port"].as<port_t>()
                                              : mqlqd::cfg::port };

    Fclient fclient{ addr, port };
    // initialize file client.
    rc = fclient.init();
    if (rc != rc::SUCCESS) {
      return rc;
    }

    // attempt to send info of the upcoming transmission of the files.
    rc = fclient.send_files_info(vfinfo);
    if (rc != rc::SUCCESS) {
      return rc;
    }

    // server is ready to accept provided files => start sending files.
    rc = fclient.send_files(vfiles);
    if (rc != rc::SUCCESS) {
      return rc;
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
