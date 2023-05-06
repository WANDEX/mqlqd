// handler of the system signals.
// main purpose of which -> finish program gracefully (correctly)
// upon receiving one of the known signals.

#include "sig_handler.hpp"

#include "sig_to_str.hpp"       // enum class SIG with fmt format specialization

#include <csignal>              // sigaction, SIGRTMAX
#include <string_view>


namespace mqlqd {

void sig_print(int sig, std::string_view const& extra_msg="")
{
  fmt::print(stderr, "\nSIG {:2}: [{}] {} {}\n\n", sig, mqlqd::SIG(sig), "signal caught.", extra_msg);
}

/**
 * ref: sigaction(2), sigaction(3p)
 * SIGKILL or SIGSTOP "cannot be caught or ignored",
 * ancestors probably meant: sould not be caught and ignored!?.
 * so that even the most disgusting program can be killed etc. (i guess).
 */
void sig_handler(int sig)
{
  switch (mqlqd::SIG(sig)) {
  // default behavior for the all not explicitly handled signals =>
  // print caught signal and finish program gracefully.
  default: sig_print(sig, "FINISH HIM!");
  }
}

struct sigaction siga {};

/**
 * @brief set the sig_handler function as the handler for all possible signals.
 * (except SIGKILL & SIGSTOP) ref: sigaction(2), signal(2), signal(7).
 * descriptions took from the signal(7).
 */
void sig_handler_set(void (*sig_handler)(int sig))
{
  siga.sa_handler = sig_handler;
  for (int sig = 1; sig <= SIGRTMAX; ++sig) {
    // skip / do not install handlers on the specific signals:
    switch (mqlqd::SIG(sig)) {
    case SIG::TRAP  : continue; // XXX OK?: Trace/breakpoint trap
    case SIG::KILL  : continue; // Kill signal  (should not be caught)
    case SIG::CONT  : continue; // Continue if stopped
    // Stop signals:
    case SIG::STOP  : continue; // Stop process (should not be caught)
    case SIG::TSTP  : continue; // Stop typed at terminal
    case SIG::TTIN  : continue; // Terminal input  for background process
    case SIG::TTOU  : continue; // Terminal output for background process
    // Ign signals
    case SIG::CHLD  : continue; // Child stopped or terminated
    case SIG::URG   : continue; // Urgent condition on socket
    case SIG::WINCH : continue; // Window resize signal (TERMINAL resize event)
    default:; // default in order to silence [-Wswitch].
    } // ^ And also we do not want to list all members of the enum.

    // finish our program gracefully (correctly) on all other signals.
    // potentially this might return -1 and set errno, but we do not care.
    sigaction(sig, &siga, nullptr);
  }
}

void sig_handler()
{
  sig_handler_set(sig_handler);
}

} // namespace mqlqd

