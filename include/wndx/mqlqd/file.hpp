#pragma once

#include "aliases.hpp" // IWYU pragma: keep

#include "wndx/sane/file.hpp"


namespace wndx::mqlqd::file {

using namespace wndx::sane::file;

/// \brief length chosen arbitrarily to fit most of of the file names into.
static constexpr std::size_t fname_max_len{ 79 };

/// \brief struct file info.
struct Finfo
{
  std::size_t m_block_size{ 0 }; // NOLINTNEXTLINE(*-avoid-c-arrays)
  char        m_fname[fname_max_len]{ "mqlqd_default_file_name\0" };
};

class File : public wndx::sane::file::File
{
public:
  File()                       = delete;
  File(File&&)                 = default;
  File(File const&)            = default;
  File& operator=(File&&)      = delete;
  File& operator=(File const&) = delete;
  ~File() noexcept             = default;

  /// \brief construct class instance from the file path & its size.
  explicit File(fs::path fpath, std::size_t sz) noexcept;

  /// \brief construct class instance from the file info structure.
  explicit File(Finfo const& finfo, fs::path dpath) noexcept;

  /// \brief convert essentials of the instance into file info structure.
  [[nodiscard]] Finfo to_finfo() const noexcept;
};

} // namespace wndx::mqlqd::file


/// \brief fmt formatter specialization for Finfo.
template <>
struct fmt::formatter<wndx::mqlqd::file::Finfo> : formatter<string_view>
{
  auto format(wndx::mqlqd::file::Finfo const& f, format_context& ctx) const
      -> format_context::iterator;
};

/// \brief fmt formatter specialization for File.
template <>
struct fmt::formatter<wndx::mqlqd::file::File> : formatter<string_view>
{
  auto format(wndx::mqlqd::file::File const& f, format_context& ctx) const
      -> format_context::iterator;
};
