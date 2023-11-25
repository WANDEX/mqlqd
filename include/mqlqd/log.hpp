#pragma once
// Logger class common for the both targets: client & daemon

#include "log_level.hpp"

#include "aliases.hpp"
#include "config.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#include <string_view>


namespace mqlqd {

class Logger final
{
public:
  Logger() = default;
  Logger(Logger &&) = default;
  Logger(const Logger &) = default;
  Logger &operator=(Logger &&) = default;
  Logger &operator=(const Logger &) = default;
  ~Logger() noexcept;

  explicit Logger(fs::path const& log_fpath) noexcept;

private:
  /**
   * @brief trace with message content - line: file.
   * for the messages of specific log level (LL:WARN, LL::ERRO, LL:CRIT).
   */
  void trace_to_the_file(
      char const* const file, int const line, LL const ll);

  void wrapper_fmt_args_helper(
      char const* const file, int const line, LL const ll,
      fmt::string_view const fmt, fmt::format_args const args);

public:
  template <typename... T>
  void wrapper_fmt_args(
      const char* const file, int const line, LL const ll,
      fmt::format_string<T...> const fmt, T&&... args)
  {
    if (m_urgency_level > ll) {
      return;
    }
    wrapper_fmt_args_helper(file, line, ll,
        fmt::format("[{}]: {}", ll, fmt),
        fmt::make_format_args(args...));
  }

public:
  [[nodiscard]] fs::path
  get_log_fpath() noexcept;

  [[nodiscard]] LL
  get_urgency() noexcept;

  void set_urgency(LL const ll) noexcept;

  /**
   * @brief specific log message format for the errno.
   * 'https://en.cppreference.com/w/cpp/error/errno'
   */
  void errnum(int const errnum, std::string_view const msg) noexcept;

private:
  fs::path m_log_fpath     { cfg::log_fpath };
  LL       m_urgency_level { cfg::urgency   };
};

/**
 * defined here for convenience => be able to use it in other files/functions ->
 * without the need to pass it into every single function as the extra argument.
 * (usually logger is needed for the whole life of the program anyway!) */
inline Logger log_g {};

#define WNDX_LOG(log_level, format, ...) \
log_g.wrapper_fmt_args(__FILE__, __LINE__, log_level, format, __VA_ARGS__)

} // namespace mqlqd

