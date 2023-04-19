#pragma once
// project-wide type aliases

#include <cstdint>              // types, MIN/MAX, etc

#include <filesystem>
#include <string_view>

namespace mqlqd {

// @brief wrapper for forced computation at compile time.
// the destructor of the type used in the function must be constexpr.
inline consteval auto cmpl_time(auto value) { return value; }

namespace fs = std::filesystem; // NOLINT(misc-unused-alias-decls)

// easiest way to access the suffixes: s, sv, etc.
using namespace std::literals;

// alias for shortness & consistency across the project codebase.
enum class LogLevel; // forward declare
using LL = LogLevel;

#if 1
using i8  = int_least8_t;
using i16 = int_least16_t;
using i32 = int_least32_t;
using i64 = int_least64_t;

using u8  = uint_least8_t;
using u16 = uint_least16_t;
using u32 = uint_least32_t;
using u64 = uint_least64_t;
#else
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#endif

/*
 * NOTE: I heard that it is not very good idea to alias standard types.
 *       But i wanted to see what will happen and how code will look like.
 *       Maybe it is actually not that bad? (What if rumors are wrong...)
 */
using sv_t = std::string_view;
using sz_t = std::size_t;

// using addr_t = std::string;
// using port_t = int;

// NOTE: cxxopts default_value() accepts std::string.
// Also, looks like any other types make life unnecessarily harder working with the cxxopts.
// => Common type for the command line options.
using cmd_opt_t = std::string;

using addr_t = sv_t;
using port_t = u16;

/*
 * Following for the consistency with the (POSIX)
 * Linux User Socket API. [$ man 3type sockaddr ] etc.
 */
// NOTE: cannot be redefined! INADDR_ANY will not allow that!
// using in_addr_t = addr_t;
// using in_port_t = port_t;

// using socklen_t   = u32; // at least 32 bits.
// using sa_family_t = u32; // u but ? bits (how small can be to be safe?)


} // namespace mqlqd

/**
 * In the end of the file after defining project-wide aliases.
 * For the convenience:
 * To not include logger declarations separately in each translation unit.
 */
#include "log.hpp"

