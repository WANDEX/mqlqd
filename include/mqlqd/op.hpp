#pragma once
// cxxopts & options related code

#include "aliases.hpp"

#include <cxxopts.hpp>

#include <string>
#include <string_view>


namespace mqlqd {

/**
 * defined here for convenience => be able to use it in other files/functions ->
 * without the need to pass it into every single function as the extra argument.
 * (usually cmd options are needed for the whole life of the program anyway!) */
inline cxxopts::ParseResult opts_g;


[[nodiscard]] cmd_opt_t
dvw(std::string_view const& value);

[[nodiscard]] cmd_opt_t
opt_wrap(std::string_view const& in_opt);

[[nodiscard]] fs::path
opt_file_wrap(std::string_view const& in_opt);

} // namespace mqlqd

