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
    options.custom_help("[OPTIONS]");
    options.positional_help("[file paths as trailing arguments ...]");
    options.set_width(80);
    options.add_options()
      ("a,addr", "Server IP address with the mqlqd_daemon.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_addr)))

      ("p,port", "Port number of the daemon on the server.",
       cxxopts::value<cmd_opt_t>()->default_value(dvw(cfg::def_port)))

      ("c,cat",  "Print file content (cat like utility mode).")
      ("f,file", "File path of the file to transmit.",
       cxxopts::value<std::vector<fs::path>>())
      ("h,help", "Show usage help.")
      ("u,urge", "Log urgency level. (All messages </> Only critical)",
       cxxopts::value<int>(), "1-7")
      ("files_trail", "File path(s) as trailing argument(s).",
       cxxopts::value<std::vector<fs::path>>());
    // to support file paths supplied as trailing arguments:
    // e.g. (after all options) file1.txt file2.txt file3.txt
    options.parse_positional({"files_trail"});

    // initialize global options variable
    opts_g = options.parse(argc, argv);

    // initialize logger with the specific log file.
    log_g  = Logger{ "/tmp/mqlqd/logs/client.log"sv };

    if (opts_g.count("help")) {
      std::cout << options.help() << '\n';
      exit(0);
    }

    if (!opts_g.count("file") && !opts_g.count("files_trail")) {
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

    // total number of file paths passed via all options: via option & positional arguments.
    const std::size_t n_files_passed{ opts_g.count("file") + opts_g.count("files_trail") };
    // vector of files info (subset of the File classes, helper info for the transmission)
    std::vector<file::mqlqd_finfo> vfinfo;
    vfinfo.reserve(n_files_passed);
    // vector of class instances
    std::vector<file::File> vfiles;
    vfiles.reserve(n_files_passed);

    if (opts_g.count("file")) {
      // add files passed via -f --file cmd options.
      for (fs::path const &fp : opts_g["file"].as<std::vector<fs::path>>()) {
        vfiles.emplace_back(file::File{ fp, fs::file_size(fp) });
      }
    }
    if (opts_g.count("files_trail")) {
      // add files passed via trailing cmd arguments.
      for (fs::path const &fp : opts_g["files_trail"].as<std::vector<fs::path>>()) {
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
      auto brc{ file.read_to_block() };
      if (brc != 0) {
        log_g.msg(LL::ERRO, fmt::format("Fail file.read_to_block() -> {}", brc));
        return brc;
      } else {
        if (opts_g.count("cat")) {
          file.print_fcontent();
        } else {
          vfinfo.emplace_back(file.to_finfo());
        }
      }
    }

    // if we are in the cat mode -> simply finish =>
    // as user do not need to initialize file client & do transmission.
    if (opts_g.count("cat")) {
      return 0;
    }

#if 1
    for(auto const& finfo : vfinfo) {
      std::cerr  << finfo << '\n';
    }
    return 8; // XXX
#endif

    Fclient fclient;
    // initialize file client.
    int fc_rc = fclient.init();
    if (fc_rc != 0) {
      log_g.msg(LL::ERRO, fmt::format("fclient.init() -> {}", fc_rc));
      return fc_rc;
    }
    // TODO: send info structures of the files.
    // TODO: if server is ready to accept provided files => begin files transfer.
    // TODO: if server rejected request on transmission => notify user about that.



    // XXX: obsolete rewrite or delete me!
    /*
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

