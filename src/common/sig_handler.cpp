// handler of the system signals.
// main purpose of which -> finish program gracefully
// upon receiving one of the known signals.

#include "sig_handler.hpp"

#include "sig_to_str.hpp"       // enum class SIG with fmt format specialization

#include <csignal>              // sigaction, SIGRTMAX


namespace mqlqd {

void sig_print(int sig)
{
  fmt::print(stderr, "\nSIG {:2}: [{}] {}\n\n", sig, mqlqd::SIG(sig), "signal caught. FINISH HIM!");
}

void sig_handler(int sig)
{
  sig_print(sig);
}

struct sigaction siga {};

// @brief set the sig_handler function as the handler for all possible signals.
// (except SIGKILL & SIGSTOP) ref: sigaction(2), signal(2), signal(7).
void sig_handler_set(void (*sig_handler)(int sig))
{
  siga.sa_handler = sig_handler;
  for (int sig = 1; sig <= SIGRTMAX; ++sig) {
    // this might return -1 and set errno, but we do not care
    sigaction(sig, &siga, nullptr);
  }
}

void sig_handler()
{
  sig_handler_set(sig_handler);
}

} // namespace mqlqd

