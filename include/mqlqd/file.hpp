#pragma once

#include "aliases.hpp"          // sv_t & other project-wide aliases

#include <filesystem>

namespace mqlqd {
namespace file {

namespace fs = std::filesystem;

[[nodiscard]]
int is_r(fs::path const& fpath) noexcept;

// TODO: DOUBTS: should i use enum instead of simple int rc - return codes?

class File final
{
public:
  inline static constexpr auto openmode{
    std::ios::in | std::ios::binary | std::ios::ate
  };

  File() = delete;
  File(File &&) = delete;
  File(const File &) = delete;
  File &operator=(File &&) = delete;
  File &operator=(const File &) = delete;
  ~File() noexcept;

  explicit File(fs::path const& fpath, const std::size_t sz) noexcept;

  void heap_cleanup() noexcept;

  [[nodiscard]]
  int heap_alloc() noexcept;

  [[nodiscard]]
  int fcontent();

  [[nodiscard]]
  int read_to_block();

  void print_fcontent() const;

  // TODO: DOUBTS: clone file permissions
  // void clone_perms();


private:
  fs::path    m_fpath{ };
  std::size_t m_block_size{ 0 };
  char       *m_block{ nullptr }; // memory block -> contiguous chunk of memory.
};

} // namespace file
} // namespace mqlqd

