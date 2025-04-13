#pragma once

#include "aliases.hpp"

#include <fmt/format.h>         // for the fmt formatter specialization of the custom types.

#include <filesystem>


namespace mqlqd {
namespace file {

// value chosen arbitrarily ->
// (to fit the largest number of file names into the array)
static constexpr size_t fname_max_len{ 79 };

// @brief struct file info.
struct mqlqd_finfo {
  size_t block_size{ 0 };
  char   fname[fname_max_len]{ "mqlqd_default_file_name" };
};

[[nodiscard]] int
is_r(fs::path const& fpath) noexcept;

[[nodiscard]] int
mkdir(fs::path const& dpath, fs::perms const& perms = fs::perms::group_all, bool force=false) noexcept;


class File final
{
public:
  /**
   * to read/write/store all files as binary data with the unified underlying type
   * across the project code base and across devices.
   * NOTE: tried other types, specifically: u8 - unsigned char type => break the logic:
   *      printing/writing binary file contents. (send/recv maybe also, but not sure!)
   *      Currently i do not see any benefit in wasting time on making this possible using u8.
   *      Probably some extra handling required, which only overcomplicate code. (so why bother?)
   */
  using char_type = char;

  inline static constexpr auto openmode_r{
    std::ios::in  | std::ios::binary | std::ios::ate
  };

  inline static constexpr auto openmode_w{
    std::ios::out | std::ios::binary
  };

  File() = default;
  File(File &&) = default;
  File(const File &) = default;
  File &operator=(File &&) = delete;
  File &operator=(const File &) = delete;
  ~File() noexcept;

  explicit File(fs::path const& fpath, const std::size_t sz) noexcept;

  // @brief construct class instance from the file info structure.
  explicit File(mqlqd_finfo const& finfo, fs::path dpath) noexcept;

  // @bief convert essentials of the instance into file info structure.
  [[nodiscard]] mqlqd_finfo
  to_finfo() const noexcept;

  /**
   * @brief for writing file to the disk.
   *
   * mostly for files constructed from the file info structure.
   */
  [[nodiscard]] int
  write();


  void heap_cleanup() noexcept;

  [[nodiscard]]
  int heap_alloc() noexcept;

  [[nodiscard]]
  int fcontent();

  [[nodiscard]]
  int read_to_block();

  void print_fcontent() const noexcept;

  // TODO: DOUBTS: clone file permissions
  // void clone_perms();

  friend std::ostream& operator<<(std::ostream& os, File const &file) {
    return os << "File file.fpath : " << file.m_fpath << '\n'
              << "file.block_size : " << '[' << file.m_block_size << ']' << '\n';
  }

public:
  fs::path    m_fpath{ };
  std::size_t m_block_size{ 0 };
  char_type  *m_block{ nullptr }; // memory block -> contiguous chunk of memory.
};

} // namespace file
} // namespace mqlqd

// overload for the std::ostream (to print file info structure in the readable text form)
inline std::ostream& operator<<(std::ostream& os, mqlqd::file::mqlqd_finfo const &finfo) {
  return os << "     finfo.fname: " << finfo.fname << '\n'
            << "finfo.block_size: " << '[' << finfo.block_size << ']' << '\n';
}

// fmt formatter specialization for finfo (one-liner -> more terse + also faster than ostream)
template <> struct fmt::formatter<mqlqd::file::mqlqd_finfo> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  template <typename FormatContext>
  auto format(mqlqd::file::mqlqd_finfo const& finfo, FormatContext &ctx) const {
    return format_to(ctx.out(), "[{}] {}", finfo.block_size, finfo.fname);
  }
};

// fmt formatter specialization for File (one-liner -> more terse + also faster than ostream)
template <> struct fmt::formatter<mqlqd::file::File> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  template <typename FormatContext>
  auto format(mqlqd::file::File const& file, FormatContext &ctx) const {
    return format_to(ctx.out(), "[{}] {}", file.m_block_size, file.m_fpath.c_str());
  }
};

