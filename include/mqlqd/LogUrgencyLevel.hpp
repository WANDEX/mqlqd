#pragma once
// Logger message urgency levels.

namespace mqlqd {

// log msg can have different log levels.
// the bigger the level, the less messages:
enum class LogUrgencyLevel
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

