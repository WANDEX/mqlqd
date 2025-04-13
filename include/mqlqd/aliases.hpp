#pragma once
// project-wide type aliases

#include "wndx/sane/aliases.hpp"

#include <cstdint>              // types, MIN/MAX, etc

#include <filesystem>
#include <string_view>

namespace mqlqd {
using namespace wndx;

/*
 * NOTE: I heard that it is not very good idea to alias standard types.
 *       But i wanted to see what will happen and how code will look like.
 *       Maybe it is actually not that bad? (What if rumors are wrong...)
 */
using sv_t = std::string_view;
using sz_t = std::size_t;

using addr_t = sv_t;
using port_t = u16;

// NOTE: cxxopts default_value() accepts std::string.
// Also, looks like any other types make life unnecessarily harder working with the cxxopts.
// => Common type for the command line options.
using cmd_opt_t = std::string;

} // namespace mqlqd


// FIXME: this is a dirty solution
/**
 * In the end of the file after defining project-wide aliases.
 * For the convenience:
 * To not include logger declarations separately in each translation unit.
 */
#include "wndx/sane/log.hpp"

