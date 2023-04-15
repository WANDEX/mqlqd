
#include "log.hpp"

#include "aliases.hpp"
#include "LogUrgencyLevel.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
// #include <fmt/std.h>            // for formatting std::filesystem::path
// https://fmt.dev/latest/api.html#standard-library-types-formatting ^
// #include <fmt/ostream.h>

#include <iostream>
// #include <filesystem>
// #include <utility>              // std::forward


namespace mqlqd {

Logger::Logger(fs::path const& log_fpath) noexcept
  : m_log_fpath{ log_fpath }
{
}

Logger::~Logger() noexcept
{
}

void Logger::set_urgency(LL const ll) noexcept
{
  m_urgency_level = ll;
  log_g.msg(LL::NTFY, fmt::format("forced urgency level = {}.", ll));
  // TODO: replace with this when possible.
  // log_g.msg(LL::NTFY, "forced urgency level = {}.\n", ll);
}


void Logger::msg(LL const ll, sv_t const& msg) noexcept
{
  if (m_urgency_level > ll) {
    return;
  }
  // print log message into the stderr stream,
  // log level (enum) name in the text form.
  fmt::print(stderr, "[{}]: {}\n", ll, msg);

  // TODO: also write message into the log file.
  // XXX : maybe use this?
  // https://github.com/PlatformLab/NanoLog
}

void Logger::msg(LL const ll, fmt::string_view format, fmt::format_args args)
{
  if (m_urgency_level > ll) {
    return;
  }
  fmt::vprint(stderr, fmt::format("[{}]: {}", ll, format), args); // OK
  // TODO: find way a way to pass variable amount of args
  // differently more convenient ... (spent too much time on this!)
  // TODO: also write message into the log file.
}

} // namespace mqlqd
