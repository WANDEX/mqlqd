#pragma once

#include "aliases.hpp" // sv_t & other project-wide aliases

#include <filesystem>
#include <string_view>

namespace mqlqd {
namespace file {

namespace fs = std::filesystem;

int is_r(fs::path const& fpath) noexcept;

int fcontent(char *mem_block, fs::path const& fpath);

// TODO: clone file permisssions
// void clone_perms();

} // namespace file
} // namespace mqlqd

