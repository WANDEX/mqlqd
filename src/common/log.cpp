// Logger implementation

#include "log.hpp"

#include "aliases.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#include <cerrno>               // for errno messages std::strerror


// toggle showing of the line number : file path
#ifndef WNDX_LOG_TRACE_TO_THE_FILE
#define WNDX_LOG_TRACE_TO_THE_FILE 1
#endif // !WNDX_LOG_TRACE_TO_THE_FILE


namespace mqlqd {

Logger::Logger(fs::path const& log_fpath) noexcept
  : m_log_fpath{ log_fpath }
{
}

Logger::~Logger() noexcept
{
}

void Logger::trace_to_the_file(
    char const* const file, int const line, LL const ll)
{
  if (ll < LL::WARN) {
    return;
  }
  fmt::print(stderr, "{:>6}: \"{}\"\n", line, file);
}

void Logger::wrapper_fmt_args_helper(
    char const* const file, int const line, LL const ll,
    fmt::string_view const fmt, fmt::format_args const args)
{
  fmt::vprint(stderr, fmt, args);
  // TODO: also write message into the log file.
  // XXX : maybe use this?
  // https://github.com/PlatformLab/NanoLog
#if WNDX_LOG_TRACE_TO_THE_FILE
  trace_to_the_file(file, line, ll);
#endif
}

[[nodiscard]] fs::path
Logger::get_log_fpath() noexcept
{
  return m_log_fpath;
}

[[nodiscard]] LL
Logger::get_urgency() noexcept
{
  return m_urgency_level;
}

void Logger::set_urgency(LL const ll) noexcept
{
  m_urgency_level = ll;
  WNDX_LOG(LL::NTFY, "forced urgency level = {}\n", ll);
}

void Logger::errnum(int const errnum, std::string_view const msg) noexcept
{
  WNDX_LOG(LL::CRIT, "{}\n\terrno: {}\n", msg, std::strerror(errnum));
}

} // namespace mqlqd

