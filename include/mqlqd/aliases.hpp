#pragma once
// project-wide type aliases

#include "wndx/sane/aliases.hpp"

namespace mqlqd {
using namespace wndx;

using addr_t = sv_t;
using port_t = u16;

// NOTE: cxxopts default_value() accepts std::string.
// Also, looks like any other type make life unnecessarily harder working with the cxxopts.
// => Common type for the command line options.
using cmd_opt_t = std::string;

} // namespace mqlqd

// In the end of the file after defining project-wide aliases. For the convenience.
// To not include logger declarations separately in each translation unit.
#include "wndx/sane/log.hpp"

