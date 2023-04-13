// daemon entry point (main)

namespace mqlqd {

[[nodiscard]] int
cmd_opts(int argc, const char *argv[]);

} // namespace mqlqd


int main(int argc, const char *argv[])
{
  using namespace mqlqd;
  int return_code{ -1 }; // also known as the error code
  return_code = cmd_opts(argc, argv);
  if (return_code != 0) return return_code;
  // potential place for the extra steps (for the future build up)

  return 0;
}

