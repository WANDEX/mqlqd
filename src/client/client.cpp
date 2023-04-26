// client entry point (main)

namespace mqlqd {

void sig_handler();

[[nodiscard]] int
cmd_opts(int argc, const char *argv[]);

} // namespace mqlqd


int main(int argc, const char *argv[])
{
  int rc{ -1 }; // return code also known as the error code
  mqlqd::sig_handler();
  rc = mqlqd::cmd_opts(argc, argv);
  if (rc != 0) return rc;

  return 0;
}

