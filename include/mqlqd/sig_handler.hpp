#pragma once

#include <string_view>

namespace mqlqd {

void sig_print(int sig, std::string_view const extra_msg="");

void sig_handler(int sig);

void sig_handler_set(void (*sig_handler)(int sig));

void sig_handler();

} // namespace mqlqd

