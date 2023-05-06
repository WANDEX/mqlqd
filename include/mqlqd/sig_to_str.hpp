#pragma once
// enum class SIG with fmt format specialization.

#include <fmt/format.h>


namespace mqlqd {

/**
 * names and respective values were copied from the htop program interface.
 * duplicate values commented out.
 * NOTE: removed 'SIG' in the beginning of each signal name
 * in order to aboid name collisions with the
 * predefined macro constants from csignal / signal.h .
 */
enum class SIG
{
  HUP      =  1,
  INT      =  2,
  QUIT     =  3,
  ILL      =  4,
  TRAP     =  5,
  ABRT     =  6,
//IOT      =  6,
  BUS      =  7,
  FPE      =  8,
  KILL     =  9,
  USR1     = 10,
  SEGV     = 11,
  USR2     = 12,
  PIPE     = 13,
  ALRM     = 14,
  TERM     = 15,
  STKFLT   = 16,
  CHLD     = 17,
  CONT     = 18,
  STOP     = 19,
  TSTP     = 20,
  TTIN     = 21,
  TTOU     = 22,
  URG      = 23,
  XCPU     = 24,
  XFSZ     = 25,
  VTALRM   = 26,
  PROF     = 27,
  WINCH    = 28,
//IO       = 29,
  POLL     = 29,
  PWR      = 30,
  SYS      = 31,
  RTMIN    = 34,
  RTMIN_1  = 35,
  RTMIN_2  = 36,
  RTMIN_3  = 37,
  RTMIN_4  = 38,
  RTMIN_5  = 39,
  RTMIN_6  = 40,
  RTMIN_7  = 41,
  RTMIN_8  = 42,
  RTMIN_9  = 43,
  RTMIN_10 = 44,
  RTMIN_11 = 45,
  RTMIN_12 = 46,
  RTMIN_13 = 47,
  RTMIN_14 = 48,
  RTMIN_15 = 49,
  RTMIN_16 = 50,
  RTMIN_17 = 51,
  RTMIN_18 = 52,
  RTMIN_19 = 53,
  RTMIN_20 = 54,
  RTMIN_21 = 55,
  RTMIN_22 = 56,
  RTMIN_23 = 57,
  RTMIN_24 = 58,
  RTMIN_25 = 59,
  RTMIN_26 = 60,
  RTMIN_27 = 61,
  RTMIN_28 = 62,
  RTMIN_29 = 63,
  RTMIN_30 = 64,
};

} // namespace mqlqd

// NOTE: fmt demands to specialize/declare this here!
// [template_spec_redecl_out_of_scope].
// @brief format enum SIG in the readable text form.
template <> struct
fmt::formatter<mqlqd::SIG> : formatter<string_view> {
  // parse is inherited from formatter<string_view>.
  using SIG =  mqlqd::SIG;
  template <typename FormatContext>
  auto format(SIG sig, FormatContext& ctx) const {
    string_view name = "unknown";
    switch (sig) {
    case SIG::HUP     : name = "SIGHUP"      ; break;
    case SIG::INT     : name = "SIGINT"      ; break;
    case SIG::QUIT    : name = "SIGQUIT"     ; break;
    case SIG::ILL     : name = "SIGILL"      ; break;
    case SIG::TRAP    : name = "SIGTRAP"     ; break;
    case SIG::ABRT    : name = "SIGABRT"     ; break;
//  case SIG::IOT     : name = "SIGIOT"      ; break;
    case SIG::BUS     : name = "SIGBUS"      ; break;
    case SIG::FPE     : name = "SIGFPE"      ; break;
    case SIG::KILL    : name = "SIGKILL"     ; break;
    case SIG::USR1    : name = "SIGUSR1"     ; break;
    case SIG::SEGV    : name = "SIGSEGV"     ; break;
    case SIG::USR2    : name = "SIGUSR2"     ; break;
    case SIG::PIPE    : name = "SIGPIPE"     ; break;
    case SIG::ALRM    : name = "SIGALRM"     ; break;
    case SIG::TERM    : name = "SIGTERM"     ; break;
    case SIG::STKFLT  : name = "SIGSTKFLT"   ; break;
    case SIG::CHLD    : name = "SIGCHLD"     ; break;
    case SIG::CONT    : name = "SIGCONT"     ; break;
    case SIG::STOP    : name = "SIGSTOP"     ; break;
    case SIG::TSTP    : name = "SIGTSTP"     ; break;
    case SIG::TTIN    : name = "SIGTTIN"     ; break;
    case SIG::TTOU    : name = "SIGTTOU"     ; break;
    case SIG::URG     : name = "SIGURG"      ; break;
    case SIG::XCPU    : name = "SIGXCPU"     ; break;
    case SIG::XFSZ    : name = "SIGXFSZ"     ; break;
    case SIG::VTALRM  : name = "SIGVTALRM"   ; break;
    case SIG::PROF    : name = "SIGPROF"     ; break;
    case SIG::WINCH   : name = "SIGWINCH"    ; break;
//  case SIG::IO      : name = "SIGIO"       ; break;
    case SIG::POLL    : name = "SIGPOLL"     ; break;
    case SIG::PWR     : name = "SIGPWR"      ; break;
    case SIG::SYS     : name = "SIGSYS"      ; break;
    case SIG::RTMIN   : name = "SIGRTMIN"    ; break;
    case SIG::RTMIN_1 : name = "SIGRTMIN+1"  ; break;
    case SIG::RTMIN_2 : name = "SIGRTMIN+2"  ; break;
    case SIG::RTMIN_3 : name = "SIGRTMIN+3"  ; break;
    case SIG::RTMIN_4 : name = "SIGRTMIN+4"  ; break;
    case SIG::RTMIN_5 : name = "SIGRTMIN+5"  ; break;
    case SIG::RTMIN_6 : name = "SIGRTMIN+6"  ; break;
    case SIG::RTMIN_7 : name = "SIGRTMIN+7"  ; break;
    case SIG::RTMIN_8 : name = "SIGRTMIN+8"  ; break;
    case SIG::RTMIN_9 : name = "SIGRTMIN+9"  ; break;
    case SIG::RTMIN_10: name = "SIGRTMIN+10" ; break;
    case SIG::RTMIN_11: name = "SIGRTMIN+11" ; break;
    case SIG::RTMIN_12: name = "SIGRTMIN+12" ; break;
    case SIG::RTMIN_13: name = "SIGRTMIN+13" ; break;
    case SIG::RTMIN_14: name = "SIGRTMIN+14" ; break;
    case SIG::RTMIN_15: name = "SIGRTMIN+15" ; break;
    case SIG::RTMIN_16: name = "SIGRTMIN+16" ; break;
    case SIG::RTMIN_17: name = "SIGRTMIN+17" ; break;
    case SIG::RTMIN_18: name = "SIGRTMIN+18" ; break;
    case SIG::RTMIN_19: name = "SIGRTMIN+19" ; break;
    case SIG::RTMIN_20: name = "SIGRTMIN+20" ; break;
    case SIG::RTMIN_21: name = "SIGRTMIN+21" ; break;
    case SIG::RTMIN_22: name = "SIGRTMIN+22" ; break;
    case SIG::RTMIN_23: name = "SIGRTMIN+23" ; break;
    case SIG::RTMIN_24: name = "SIGRTMIN+24" ; break;
    case SIG::RTMIN_25: name = "SIGRTMIN+25" ; break;
    case SIG::RTMIN_26: name = "SIGRTMIN+26" ; break;
    case SIG::RTMIN_27: name = "SIGRTMIN+27" ; break;
    case SIG::RTMIN_28: name = "SIGRTMIN+28" ; break;
    case SIG::RTMIN_29: name = "SIGRTMIN+29" ; break;
    case SIG::RTMIN_30: name = "SIGRTMIN+30" ; break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

// overload for the std::ostream (to print SIG in the readable text form)
inline std::ostream& operator<<(std::ostream& os, mqlqd::SIG const sig) {
  return os << fmt::to_string(sig);
}

