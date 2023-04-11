#pragma once

#include "aliases.hpp"

namespace mqlqd {
namespace cfg {

// NOTE: cxxopts default_value() accepts std::string

inline constexpr sv_t def_addr{ "127.0.0.1"sv }; // i.e. localhost
inline constexpr sv_t def_port{ "69042"sv };

// default (client UID / device MAC) unique to the client/device.
inline constexpr sv_t def_uid{ "f000::f000:f000:f000:f000"sv };

} // namespace cfg
} // namespace mqlqd

