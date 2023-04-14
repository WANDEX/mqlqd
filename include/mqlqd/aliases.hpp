#pragma once
// project-wide type aliases

#include <cinttypes>            // XXX format macro constants
#include <cstdint>              // types, *_MIN, etc
#include <filesystem>
#include <string_view>

namespace mqlqd {

namespace fs = std::filesystem; // NOLINT(misc-unused-alias-decls)

// easiest way to access the suffixes: s, sv, etc.
using namespace std::literals;

// using i8  = int_least8_t;
using i16 = int_least16_t;
using i32 = int_least32_t;
// using i64 = int_least64_t;

// using u8  = uint_least8_t;
using u16 = uint_least16_t;
using u32 = uint_least32_t;
// using u64 = uint_least64_t;

/*
 * NOTE: I heard that it is not very good idea to alias standard types.
 *       But i wanted to see what will happen and how code will look like.
 *       Maybe it is actually not that bad? (What if rumors are wrong...)
 */
using sv_t = std::string_view;
using sz_t = std::size_t;

using addr_t = sv_t;
using port_t = int;
// using addr_t = std::string;
// using port_t = u16;

// NOTE: cxxopts default_value() accepts std::string.
// Also, looks like any other types make life unnecessarily harder working with the cxxopts.
// => Common type for the command line options.
using cmd_opt_t = std::string;

} // namespace mqlqd

