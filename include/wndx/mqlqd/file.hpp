#pragma once

#include "aliases.hpp"        // IWYU pragma: keep

#include "wndx/sane/file.hpp" // IWYU pragma: keep

#include <cstring>            // std::memcmp()


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

class File final
{
public:
  /// to read/write/store all files as binary data with the unified underlying
  /// type across the project code base and across devices.
  /// NOTE: tried other types, specifically:
  /// u8 - unsigned char type => break the logic:
  /// printing/writing binary file contents (send/recv maybe also, but not sure)
  /// Currently I do not see any benefit in wasting time on making this
  /// possible using u8. Probably some extra handling required, which only
  /// overcomplicate code. (so why bother?)
  using char_type = char;

  // clang-format off
  static constexpr auto openmode_r{
    std::ios::in  | std::ios::binary | std::ios::ate
  };
  static constexpr auto openmode_w{
    std::ios::out | std::ios::binary
  };
  // clang-format on

  File()                       = default;
  File(File&&)                 = default;
  File(File const&)            = default;
  File& operator=(File&&)      = delete;
  File& operator=(File const&) = delete;
  ~File() noexcept;

  /// \brief construct class instance from the file path & its size.
  explicit File(fs::path fpath, std::size_t sz) noexcept;

  /// \brief construct class instance from the file info structure.
  explicit File(Finfo const& finfo, fs::path const& dpath) noexcept;

  /// \brief cleanup facilities for the heap.
  void clean_heap() noexcept;

  /// \brief convert essentials of the instance into file info structure.
  [[nodiscard]] Finfo to_finfo() const noexcept;

  /// \brief allocate required space in the heap for the file contents.
  [[nodiscard]] rc alloc() noexcept;

  /// \brief write file to the disk.
  [[nodiscard]] rc write() const noexcept;

  /// \brief read file contents into the memory block.
  [[nodiscard]] rc read() const noexcept;

  /// \brief alloc space & read file contents into the memory block.
  [[nodiscard]] rc alloc_and_read() noexcept;

  /// \brief copy file to the destination.
  [[nodiscard]] rc copy_to(fs::path const& dst) noexcept;

  /// \brief move file to the destination.
  [[nodiscard]] rc move_to(fs::path const& dst) noexcept;

  /// \brief print file contents (like a UNIX cat utility).
  void print() const noexcept;

  /// \brief path to the file.
  [[nodiscard]] fs::path path() const noexcept { return m_fpath; }

  /// \brief size of contiguous memory block required for the file.
  [[nodiscard]] constexpr std::size_t size() const noexcept
  {
    return m_block_size;
  }

  /// \brief memory block.
  [[nodiscard]] constexpr char_type* memory() const noexcept { return m_block; }

  /// \brief uses compiler intrinsics for the efficient memory comparison.
  ///
  /// \see https://en.cppreference.com/w/cpp/string/byte/memcmp
  [[nodiscard]] static int memcmp(char_type const* const lhs,
                                  char_type const* const rhs, std::size_t count)
  {
    return std::memcmp(lhs, rhs, count); // not constexpr!
  }

  /// \brief uses compiler intrinsics for comparing files memory blocks.
  [[nodiscard]] int memcmp(char_type const* const rhs) const noexcept
  {
    return memcmp(memory(), rhs, size());
  }

  /// \brief allow conversion of the memory block pointer to bool.
  constexpr explicit operator bool() const noexcept
  {
    return m_block != nullptr;
  }

  constexpr bool operator!=(File const& rhs) const noexcept
  {
    return !m_block || !rhs.m_block || size() != rhs.size();
  }

  constexpr bool operator<(File const& rhs) const noexcept
  {
    return operator!=(rhs) && size() < rhs.size();
  }

  constexpr bool operator>(File const& rhs) const noexcept
  {
    return operator!=(rhs) && size() > rhs.size();
  }

  /// \brief equality of the files contents (compare memory blocks).
  ///
  /// memory comparison is the most expensive operation,
  /// so we compare memory at the very end, after all other conditions.
  constexpr bool operator==(File const& rhs) const noexcept
  {
    return !operator!=(rhs) && memcmp(rhs.memory()) == 0;
  }

  constexpr bool operator<=(File const& rhs) const noexcept
  {
    return operator<(rhs) || operator==(rhs);
  }

  constexpr bool operator>=(File const& rhs) const noexcept
  {
    return operator>(rhs) || operator==(rhs);
  }

private:
  std::size_t m_block_size{ 0 };
  fs::path    m_fpath{};
  char_type*  m_block{ nullptr }; // memory block -> contiguous chunk of memory.
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
