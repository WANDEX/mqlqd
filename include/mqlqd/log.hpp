#pragma once
// Logger class common for the both targets: client & daemon

#include "log_level.hpp"

#include "aliases.hpp"
#include "config.hpp"

#include <fmt/format.h>


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

  void set_urgency(LL const ll) noexcept;

  // msg - explicit level of urgency
  void msg(LL const ll, sv_t const& message) noexcept;

  void msg(LL const ll, fmt::string_view format, fmt::format_args args);

  [[nodiscard]] fs::path
  get_log_fpath();

private:
  // std::shared_ptr<Logger> m_lptr{ nullptr };
  fs::path m_log_fpath{ cfg::log_fpath };
  LL m_urgency_level  { cfg::urgency };

};

/**
 * defined here for convenience => be able to use it in other files/functions ->
 * without the need to pass it into every single function as the extra argument.
 * (usually logger is needed for the whole life of the program anyway!) */
inline Logger log_g {};

} // namespace mqlqd

