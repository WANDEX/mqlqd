#pragma once
// Logger message urgency levels.

#include <fmt/format.h>


namespace mqlqd {

// log msg can have different log levels.
// the bigger the level, the less messages:
enum class LogLevel
{
  DBUG = 1, // DEBUG
  INFO,     // INFO
  STAT,     // STATUS
  NTFY,     // NOTIFY
  WARN,     // WARNING
  ERRO,     // ERROR
  CRIT,     // CRITICAL
};

} // namespace mqlqd

// NOTE: fmt demands to specialize/declare this here!
// [template_spec_redecl_out_of_scope].
// @brief format enum log levels in the readable text form.
template <> struct
fmt::formatter<mqlqd::LogLevel> : formatter<string_view> {
  // parse is inherited from formatter<string_view>.
  using LL = mqlqd::LogLevel;
  template <typename FormatContext>
  auto format(LL el, FormatContext& ctx) const {
    string_view name = "unknown";
    switch (el) {
    case LL::DBUG: name = "DBUG"; break;
    case LL::INFO: name = "INFO"; break;
    case LL::STAT: name = "STAT"; break;
    case LL::NTFY: name = "NTFY"; break;
    case LL::WARN: name = "WARN"; break;
    case LL::ERRO: name = "ERRO"; break;
    case LL::CRIT: name = "CRIT"; break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

// overload for the std::ostream (to print LogLevel in the readable text form)
inline std::ostream& operator<<(std::ostream& os, mqlqd::LogLevel const ll) {
  return os << fmt::to_string(ll);
}

