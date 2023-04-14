#pragma once
// Logger class common for the both targets: client & daemon

#include "aliases.hpp"
#include "config.hpp"

#include <memory>               // std::shared_ptr


namespace mqlqd {

class Logger final
{
public:
  Logger() = delete;
  Logger(Logger &&) = delete;
  Logger(const Logger &) = delete;
  Logger &operator=(Logger &&) = delete;
  Logger &operator=(const Logger &) = delete;
  ~Logger() noexcept;

  explicit Logger(fs::path const& log_fpath) noexcept;

  // msg - default level of urgency
  void msg(sv_t const& message) noexcept;
  // msg - explicit level of urgency
  void msg(LL const ll, sv_t const& message) noexcept;

  [[nodiscard]] fs::path
  get_log_fpath();

private:
  // std::shared_ptr<Logger> m_lptr{ nullptr };
  fs::path m_log_fpath{ cfg::log_fpath };
  LL m_urgency_level  { cfg::urgency };

};

} // namespace mqlqd

