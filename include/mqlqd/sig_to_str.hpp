#pragma once
// enum class SIG with fmt format specialization.

#include <fmt/format.h>


namespace mqlqd {

// @brief names and respective values were copied from the htop program interface.
// duplicate values commented out.
enum class SIG
{
  SIGHUP      =  1,
  SIGINT      =  2,
  SIGQUIT     =  3,
  SIGILL      =  4,
  SIGTRAP     =  5,
  SIGABRT     =  6,
//SIGIOT      =  6,
  SIGBUS      =  7,
  SIGFPE      =  8,
  SIGKILL     =  9,
  SIGUSR1     = 10,
  SIGSEGV     = 11,
  SIGUSR2     = 12,
  SIGPIPE     = 13,
  SIGALRM     = 14,
  SIGTERM     = 15,
  SIGSTKFLT   = 16,
  SIGCHLD     = 17,
  SIGCONT     = 18,
  SIGSTOP     = 19,
  SIGTSTP     = 20,
  SIGTTIN     = 21,
  SIGTTOU     = 22,
  SIGURG      = 23,
  SIGXCPU     = 24,
  SIGXFSZ     = 25,
  SIGVTALRM   = 26,
  SIGPROF     = 27,
  SIGWINCH    = 28,
//SIGIO       = 29,
  SIGPOLL     = 29,
  SIGPWR      = 30,
  SIGSYS      = 31,
  SIGRTMIN    = 34,
  SIGRTMIN_1  = 35,
  SIGRTMIN_2  = 36,
  SIGRTMIN_3  = 37,
  SIGRTMIN_4  = 38,
  SIGRTMIN_5  = 39,
  SIGRTMIN_6  = 40,
  SIGRTMIN_7  = 41,
  SIGRTMIN_8  = 42,
  SIGRTMIN_9  = 43,
  SIGRTMIN_10 = 44,
  SIGRTMIN_11 = 45,
  SIGRTMIN_12 = 46,
  SIGRTMIN_13 = 47,
  SIGRTMIN_14 = 48,
  SIGRTMIN_15 = 49,
  SIGRTMIN_16 = 50,
  SIGRTMIN_17 = 51,
  SIGRTMIN_18 = 52,
  SIGRTMIN_19 = 53,
  SIGRTMIN_20 = 54,
  SIGRTMIN_21 = 55,
  SIGRTMIN_22 = 56,
  SIGRTMIN_23 = 57,
  SIGRTMIN_24 = 58,
  SIGRTMIN_25 = 59,
  SIGRTMIN_26 = 60,
  SIGRTMIN_27 = 61,
  SIGRTMIN_28 = 62,
  SIGRTMIN_29 = 63,
  SIGRTMIN_30 = 64,
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
    case SIG::SIGHUP     : name = "SIGHUP"      ; break;
    case SIG::SIGINT     : name = "SIGINT"      ; break;
    case SIG::SIGQUIT    : name = "SIGQUIT"     ; break;
    case SIG::SIGILL     : name = "SIGILL"      ; break;
    case SIG::SIGTRAP    : name = "SIGTRAP"     ; break;
    case SIG::SIGABRT    : name = "SIGABRT"     ; break;
//  case SIG::SIGIOT     : name = "SIGIOT"      ; break;
    case SIG::SIGBUS     : name = "SIGBUS"      ; break;
    case SIG::SIGFPE     : name = "SIGFPE"      ; break;
    case SIG::SIGKILL    : name = "SIGKILL"     ; break;
    case SIG::SIGUSR1    : name = "SIGUSR1"     ; break;
    case SIG::SIGSEGV    : name = "SIGSEGV"     ; break;
    case SIG::SIGUSR2    : name = "SIGUSR2"     ; break;
    case SIG::SIGPIPE    : name = "SIGPIPE"     ; break;
    case SIG::SIGALRM    : name = "SIGALRM"     ; break;
    case SIG::SIGTERM    : name = "SIGTERM"     ; break;
    case SIG::SIGSTKFLT  : name = "SIGSTKFLT"   ; break;
    case SIG::SIGCHLD    : name = "SIGCHLD"     ; break;
    case SIG::SIGCONT    : name = "SIGCONT"     ; break;
    case SIG::SIGSTOP    : name = "SIGSTOP"     ; break;
    case SIG::SIGTSTP    : name = "SIGTSTP"     ; break;
    case SIG::SIGTTIN    : name = "SIGTTIN"     ; break;
    case SIG::SIGTTOU    : name = "SIGTTOU"     ; break;
    case SIG::SIGURG     : name = "SIGURG"      ; break;
    case SIG::SIGXCPU    : name = "SIGXCPU"     ; break;
    case SIG::SIGXFSZ    : name = "SIGXFSZ"     ; break;
    case SIG::SIGVTALRM  : name = "SIGVTALRM"   ; break;
    case SIG::SIGPROF    : name = "SIGPROF"     ; break;
    case SIG::SIGWINCH   : name = "SIGWINCH"    ; break;
//  case SIG::SIGIO      : name = "SIGIO"       ; break;
    case SIG::SIGPOLL    : name = "SIGPOLL"     ; break;
    case SIG::SIGPWR     : name = "SIGPWR"      ; break;
    case SIG::SIGSYS     : name = "SIGSYS"      ; break;
    case SIG::SIGRTMIN   : name = "SIGRTMIN"    ; break;
    case SIG::SIGRTMIN_1 : name = "SIGRTMIN+1"  ; break;
    case SIG::SIGRTMIN_2 : name = "SIGRTMIN+2"  ; break;
    case SIG::SIGRTMIN_3 : name = "SIGRTMIN+3"  ; break;
    case SIG::SIGRTMIN_4 : name = "SIGRTMIN+4"  ; break;
    case SIG::SIGRTMIN_5 : name = "SIGRTMIN+5"  ; break;
    case SIG::SIGRTMIN_6 : name = "SIGRTMIN+6"  ; break;
    case SIG::SIGRTMIN_7 : name = "SIGRTMIN+7"  ; break;
    case SIG::SIGRTMIN_8 : name = "SIGRTMIN+8"  ; break;
    case SIG::SIGRTMIN_9 : name = "SIGRTMIN+9"  ; break;
    case SIG::SIGRTMIN_10: name = "SIGRTMIN+10" ; break;
    case SIG::SIGRTMIN_11: name = "SIGRTMIN+11" ; break;
    case SIG::SIGRTMIN_12: name = "SIGRTMIN+12" ; break;
    case SIG::SIGRTMIN_13: name = "SIGRTMIN+13" ; break;
    case SIG::SIGRTMIN_14: name = "SIGRTMIN+14" ; break;
    case SIG::SIGRTMIN_15: name = "SIGRTMIN+15" ; break;
    case SIG::SIGRTMIN_16: name = "SIGRTMIN+16" ; break;
    case SIG::SIGRTMIN_17: name = "SIGRTMIN+17" ; break;
    case SIG::SIGRTMIN_18: name = "SIGRTMIN+18" ; break;
    case SIG::SIGRTMIN_19: name = "SIGRTMIN+19" ; break;
    case SIG::SIGRTMIN_20: name = "SIGRTMIN+20" ; break;
    case SIG::SIGRTMIN_21: name = "SIGRTMIN+21" ; break;
    case SIG::SIGRTMIN_22: name = "SIGRTMIN+22" ; break;
    case SIG::SIGRTMIN_23: name = "SIGRTMIN+23" ; break;
    case SIG::SIGRTMIN_24: name = "SIGRTMIN+24" ; break;
    case SIG::SIGRTMIN_25: name = "SIGRTMIN+25" ; break;
    case SIG::SIGRTMIN_26: name = "SIGRTMIN+26" ; break;
    case SIG::SIGRTMIN_27: name = "SIGRTMIN+27" ; break;
    case SIG::SIGRTMIN_28: name = "SIGRTMIN+28" ; break;
    case SIG::SIGRTMIN_29: name = "SIGRTMIN+29" ; break;
    case SIG::SIGRTMIN_30: name = "SIGRTMIN+30" ; break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

// overload for the std::ostream (to print SIG in the readable text form)
inline std::ostream& operator<<(std::ostream& os, mqlqd::SIG const sig) {
  return os << fmt::to_string(sig);
}

