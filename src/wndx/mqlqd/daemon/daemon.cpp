/// daemon entry point (main)

#include "wndx/sane/rc.hpp"

#include "wndx/sane/sig.hpp" // IWYU pragma: keep

namespace wndx::mqlqd {

// NOLINTNEXTLINE(*-avoid-c-arrays)
[[nodiscard]] sane::rc cmd_opts(int argc, char const* argv[]);

} // namespace wndx::mqlqd


int main(int argc, char const* argv[])
{
  using namespace wndx::sane;
  using namespace wndx::mqlqd;
  rc rc{ rc::INIT };
  sig::handler();
  rc = cmd_opts(argc, argv);
  if (rc != rc::SUCCESS) {
    return static_cast<int>(rc);
  }
  return 0;
}
