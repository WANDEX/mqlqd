
#include "wndx/mqlqd/file.hpp"


namespace wndx::mqlqd::file {

static constexpr std::string_view ctor{ "ctor - File instance" };

File::File(fs::path fpath, size_t sz) noexcept
    : wndx::sane::file::File(std::move(fpath), sz)
{
  WNDX_LOG(LL::DBUG, "{} from file & size:\n\t{}\n", ctor, *this);
}

// NOLINTNEXTLINE(performance-unnecessary-value-param)
File::File(Finfo const& finfo, fs::path dpath) noexcept
    // NOLINTNEXTLINE(*-array-to-pointer-decay, hicpp-no-array-decay)
    : wndx::sane::file::File({ dpath / std::string(finfo.m_fname) },
                             finfo.m_block_size)
{
  WNDX_LOG(LL::DBUG, "{} from Finfo & dir path:\n\t{}\n", ctor, *this);
}

[[nodiscard]] Finfo File::to_finfo() const noexcept
{
  Finfo finfo{};
  finfo.m_block_size = size();
  // fill the array - file name (element by element)
  auto fname{ path().filename().string() };
  for (size_t i = 0; i < fname.length(); ++i) {
    finfo.m_fname[i] = fname[i]; // NOLINT(*-constant-array-index)
  }
  // null terminator after the file name (important)
  finfo.m_fname[fname.length()] = '\0'; // NOLINT(*-constant-array-index)
  return finfo;
}

} // namespace wndx::mqlqd::file


// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto fmt::formatter<wndx::mqlqd::file::Finfo>::format(
    wndx::mqlqd::file::Finfo const& f, format_context& ctx) const
    -> format_context::iterator
{
  return format_to(ctx.out(), "[{}] {}", f.m_block_size, f.m_fname);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto fmt::formatter<wndx::mqlqd::file::File>::format(
    wndx::mqlqd::file::File const& f, format_context& ctx) const
    -> format_context::iterator
{
  return format_to(ctx.out(), "[{}] {}", f.size(), f.path());
}

#if WNDX_LOG_OSTREAM_SUPPORT
std::ostream& operator<<(std::ostream& os, wndx::mqlqd::file::Finfo const& f)
{
  return os << fmt::to_string(f);
}

std::ostream& operator<<(std::ostream& os, wndx::mqlqd::file::File const& f)
{
  return os << fmt::to_string(f);
}
#endif // WNDX_LOG_OSTREAM_SUPPORT
