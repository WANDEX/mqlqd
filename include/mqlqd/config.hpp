#pragma once

#include "aliases.hpp"

namespace mqlqd {
namespace cfg {

inline constexpr addr_t addr{ "127.0.0.1" };  // i.e. localhost
inline constexpr port_t port{ 42069 };        // u16 max!

// log urgency level
// inline constexpr LL urgency{ LL::NTFY }; // default for the basic users
inline constexpr LL urgency{ LL::DBUG }; // development level (all messages)

// default log file path
inline constexpr sv_t log_fpath{ "/tmp/mqlqd/logs/default.log" };

// default (client UID / device MAC) unique to the client/device.
inline constexpr sv_t def_uid{ "f000::f000:f000:f000:f000" };

/**
 * NOTE: Keep following variables up to date with the above!
 * => following variables are only for the representation
 * in the simple text form of the default config values!
 * NOTE: cxxopts default_value() accepts std::string etc.
 *
 * XXX : I think there is no need in duplicates anymore.
 *      let this note live here for a while.
 */

} // namespace cfg
} // namespace mqlqd

