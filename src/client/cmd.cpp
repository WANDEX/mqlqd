// client command line (cmd)

#include "aliases.hpp"
#include "config.hpp"
#include "file.hpp"
#include "op.hpp"

#include "fclient.hpp"

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
    cxxopts::Options options("mqlqd_client",
        "Transfer file(s) over TCP/IP to the server running the mqlqd_daemon.");
    options.set_width(80);
    options.add_options()
      ("a,addr", "Server IP address with the mqlqd_daemon.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_addr)))

      ("p,port", "Port number of the daemon on the server.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_port)))

      ("c,cat",  "Print file content (cat like utility mode).")
      ("f,file", "File path of the file to transmit.", cxxopts::value<cmd_opt_t>())
      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7");
    /*
     *   ("file_paths", "File path(s) as trailing argument(s).",
     *    cxxopts::value<std::vector<cmd_opt_t>>());
     * // to support multiple files as trailing arguments: file1.txt file2.txt file3.txt
     * options.parse_positional({"file_paths"});
     * // XXX: UNIMPLEMENTED ^
     */

    // initialize global options variable
    opts_g = options.parse(argc, argv);

    // initialize logger with the specific log file.
    log_g  = Logger{ "/tmp/mqlqd/logs/client.log"sv };

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

    if (opts_g.count("urge")) {
      // force specific log urgency level.
      // (has priority over the value in config).
      LL urgency{ opts_g["urge"].as<int>() };
      log_g.set_urgency(urgency);
    }

    // TODO: + number of fpaths passed via positional arguments.
    const std::size_t n_files_passed{ opts_g.count("file") };
    // single allocation to fit all required files without extra resizing of the vector.
    std::vector<file::File> vfiles(n_files_passed);
    // loop over each file path passed via the cmd arguments (optional + positional).
    for (size_t i = 0; i < n_files_passed; i++) {
      const fs::path fp{ opt_file_wrap("file") };
      file::File file{ fp, fs::file_size(fp) };
      /**
       * Read contents of the file into the block of memory.
       * We are doing this here to not have potential bottleneck later -> on the transmission step.
       * (especially in terms of reading speed from the users block devices e.g. Slow HDD drive etc.).
       */
      auto brc{ file.read_to_block() };
      if (brc != 0) {
        log_g.msg(LL::ERRO, fmt::format("Fail file.read_to_block() -> {}", brc));
        return brc;
      } else {
        if (opts_g.count("cat")) {
          file.print_fcontent();
        } else {
          // log_g.msg(LL::NTFY, fmt::format("[UNIMPLEMENTED] file.read_to_block() -> {}", brc));
          // FIXME: currenly pushes only one - the first passed file.
          vfiles.push_back(file);
        }
      }
    }

    // if we are in the cat mode -> simply finish =>
    // as user do not need to initialize file client & do transmission.
    if (opts_g.count("cat")) {
      return 0;
    }

    Fclient fclient;
    // initialize file client.
    int fc_rc = fclient.init();
    if (fc_rc != 0) {
      log_g.msg(LL::ERRO, fmt::format("fclient.init() -> {}", fc_rc));
    }
    // TODO: send files
    int sf_rc{ -1 };
    int sent_files_count{ 0 };
    for (const file::File &f : vfiles) {
      sf_rc = fclient.send_file(f);
      if (sf_rc != 0) {
        log_g.msg(LL::ERRO, fmt::format("Fail fclient.send_file(f) -> {}", sf_rc));
      } else {
        ++sent_files_count;
        log_g.msg(LL::STAT, fmt::format("Successfully sent file: {}", f.m_fpath.string()));
      }
    }
    log_g.msg(LL::NTFY, fmt::format("[{}/{}] files were successfully sent.",
              sent_files_count, n_files_passed));

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

