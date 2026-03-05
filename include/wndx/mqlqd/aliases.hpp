#pragma once
// project-wide type aliases

#include "wndx/sane/aliases.hpp"

namespace wndx::mqlqd {

using namespace wndx::sane;

using addr_t = sv_t;
using port_t = u16;

/// cxxopts default_value() accepts std::string.
/// Any other type make working with the cxxopts unnecessarily harder.
/// => Common type for the command line options.
using cmd_opt_t = std::string;

} // namespace wndx::mqlqd
