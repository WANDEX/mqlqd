#pragma once

#include <string_view>

bool read_file(std::string_view const& fpath);
bool send_file(std::string_view const& socket);
