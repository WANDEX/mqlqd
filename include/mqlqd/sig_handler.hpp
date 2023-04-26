#pragma once

namespace mqlqd {

void sig_print(int sig);

void sig_handler(int sig);

void sig_handler_set(void (*sig_handler)(int sig));

void sig_handler();

} // namespace mqlqd

