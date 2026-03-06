
#include "wndx/sane/sig.hpp" // IWYU pragma: keep

namespace wndx::mqlqd {

/// exclude code from the build on not supported platforms:
#ifdef _WIN32

/// \brief not supported by the platform, empty function.
void unix_sig_handlers() {}

#else  // supported platforms begin

using namespace wndx::sane::sig;

/// \brief set handlers for the Unix signals.
void unix_sig_handlers() { handlers_set_defaults(handlers); }

#endif // supported platforms end

} // namespace wndx::mqlqd
