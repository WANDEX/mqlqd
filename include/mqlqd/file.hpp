#pragma once

#include "aliases.hpp"          // sv_t & other project-wide aliases

#include <filesystem>


namespace mqlqd {
namespace file {

[[nodiscard]]
int is_r(fs::path const& fpath) noexcept;

// TODO: DOUBTS: should i use enum instead of simple int rc - return codes?

// value chosen arbitrarily ->
// (to fit the largest number of file names into the array)
static constexpr size_t fname_max_len{ 79 };

// @brief struct file info.
struct mqlqd_finfo {
  size_t block_size{ 0 };
  char   fname[fname_max_len]{ "mqlqd_default_file_name" };
  // sv_t   fname{ "default_file_name" };
  // char   *block{ nullptr }; // memory block -> contiguous chunk of memory.
};

// @brief struct file info's.
struct mqlqd_finfos {
  size_t n_files{ 0 };
  struct mqlqd_finfo *info_next{ nullptr };
};

class File final
{
public:
  inline static constexpr auto openmode_r{
    std::ios::in  | std::ios::binary | std::ios::ate
  };

  inline static constexpr auto openmode_w{
    std::ios::out | std::ios::binary
    // std::ios::out | std::ios::binary | std::ios::ate
  };

  File() = default;
  File(File &&) = default;
  File(const File &) = default;
  File &operator=(File &&) = delete;
  File &operator=(const File &) = delete;
  ~File() noexcept;

  explicit File(fs::path const& fpath, const std::size_t sz) noexcept;

  // @brief construct class instance from the file info structure.
  // TODO: IMPL
  // explicit File(fs::path const& dpath, mqlqd_finfo const& info_file) noexcept;

  /**
   * @brief for writing file to the disk.
   *
   * mostly for files constructed from the info file structure.
   */
  void write(fs::path const& dest_dir, sv_t const& fname);

  void write(fs::path const& new_file_dest_path);

  void write(sv_t const& new_file_dest_path);


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


public:
  fs::path    m_fpath{ };
  std::size_t m_block_size{ 0 };
  char       *m_block{ nullptr }; // memory block -> contiguous chunk of memory.
};

} // namespace file
} // namespace mqlqd

