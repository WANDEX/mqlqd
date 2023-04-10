#pragma once

#include "aliases.hpp"

namespace mqlqd {
namespace cfg {

inline constexpr addr_t def_addr{ "127.0.0.1"sv }; // i.e. localhost
inline constexpr port_t def_port{ 8080 };

// default (client UID / device MAC) unique to the client/device.
inline constexpr sv_t def_uid{ "f000::f000:f000:f000:f000"sv };

} // namespace cfg
} // namespace mqlqd

