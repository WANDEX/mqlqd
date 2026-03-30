#include "wndx/mqlqd/file.hpp"

#include <gtest/gtest.h>

#include <cstring> // memcpy
#include <filesystem>
#include <string>
#include <system_error>


namespace wndx::mqlqd {

[[nodiscard]]
auto make_tmp_dir() noexcept
{
  rc              rc{ rc::FAILURE };
  std::error_code ec{};
  fs::path        tmp_dir{ fs::temp_directory_path(ec) };
  if (ec) {
    WNDX_LOG(LL::ERRO, "[FAIL] fs::temp_directory_path(ec) -> v:{} m:{}\n",
             ec.value(), ec.message());
  }
  tmp_dir /= "wndx";
  rc = wndx::sane::file::mkdir(tmp_dir, fs::perms::all);
  EXPECT_TRUE(rc == rc::SUCCESS);
  tmp_dir /= "mqlqd";
  rc = wndx::sane::file::mkdir(tmp_dir, fs::perms::all);
  EXPECT_TRUE(rc == rc::SUCCESS);
  return tmp_dir.string();
}

static auto const g_tmp_dir{ make_tmp_dir() };

class File_test : public ::testing::Test
{
public:
  bool m_do_alloc{ true };

  /// \brief allocate space, read file contents, check that memory is allocated.
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  void alloc_read_check(file::File& file) const noexcept
  {
    if (m_do_alloc) {
      /// read file and check that memory is allocated.
      ASSERT_TRUE(file.alloc_and_read() == rc::SUCCESS);
      ASSERT_TRUE(file.memory() != nullptr);
      ASSERT_TRUE(file);
      ASSERT_FALSE(!file);
    } else {
      ASSERT_TRUE(file.memory() == nullptr);
      ASSERT_FALSE(file);
      ASSERT_TRUE(!file);
    }
  }

  /// \brief helper function to create File from the tests data files.
  [[nodiscard]]
  file::File
  alloc_file(std::string const& file_name = "ascii_1.txt") const noexcept
  {
    fs::path dp;
    fs::path fp;
    EXPECT_NO_THROW({
      dp = fs::absolute("data");
      fp = fs::canonical(dp / file_name);
    });
    file::File File(fp, fs::file_size(fp));
    alloc_read_check(File);
    return File;
  }

  [[nodiscard]]
  static fs::path get_tmp_dir() noexcept
  {
    return g_tmp_dir;
  }

  [[nodiscard]]
  file::File tmp_file(std::string const& fname) const noexcept
  {
    auto     file{ alloc_file(fname) };
    fs::path tmp_fpath{ get_tmp_dir() / fname };
    WNDX_LOG(LL::DBUG, "tmp_fpath: {}\n", tmp_fpath.string());
    EXPECT_TRUE(file.copy_to(tmp_fpath) == rc::SUCCESS);
    return file;
  }
};

TEST_F(File_test, ctor_file_size)
{
  auto file{ alloc_file() };
  ASSERT_EQ(file.size(), fs::file_size(file.path()));
}

TEST_F(File_test, File_to_finfo)
{
  auto const file_name{ "ascii_2.txt" }; // NOLINT(readability-qualified-auto)
  auto const file{ alloc_file(file_name) };
  auto const finfo{ file.to_finfo() };
  // NOLINTNEXTLINE(*-array-to-pointer-decay, hicpp-no-array-decay)
  ASSERT_EQ(file_name, std::string(finfo.m_fname));
}

TEST_F(File_test, ctor_finfo_path)
{
  auto const file_name{ "ascii_3.txt" }; // NOLINT(readability-qualified-auto)
  auto const len{ std::string(file_name).length() };
  auto const file{ alloc_file(file_name) };
  auto const finfo{ file.to_finfo() };
  // NOLINTNEXTLINE(*-array-to-pointer-decay, hicpp-no-array-decay)
  ASSERT_EQ(file_name, std::string(finfo.m_fname));
  ASSERT_EQ(file.size(), fs::file_size(file.path()));

  file::Finfo finfo2;
  finfo2.m_block_size = file.size();
  finfo2.m_fname[len] = '\0'; // NOLINT(*-constant-array-index)
  std::memcpy(&finfo2.m_fname, file_name, len);
  // NOLINTNEXTLINE(*-array-to-pointer-decay, hicpp-no-array-decay)
  ASSERT_EQ(std::string(finfo.m_fname), std::string(finfo2.m_fname));
}

TEST_F(File_test, compare_nullptr)
{
  m_do_alloc = false;
  auto file1{ alloc_file() };
  auto file2{ alloc_file() };
  ASSERT_FALSE(file1 < file2);
  ASSERT_FALSE(file1 > file2);
  ASSERT_TRUE(file1 != file2);  // assume not eq when nullptr!
  ASSERT_FALSE(file1 == file2); // due to nullptr!
  ASSERT_FALSE(file1 <= file2);
  ASSERT_FALSE(file1 >= file2);
}

TEST_F(File_test, compare_eq)
{
  auto file1{ alloc_file() };
  auto file2{ alloc_file() };
  ASSERT_FALSE(file1 < file2);
  ASSERT_FALSE(file1 > file2);
  ASSERT_FALSE(file1 != file2);
  ASSERT_TRUE(file1 == file2);
  ASSERT_TRUE(file1 <= file2);
  ASSERT_TRUE(file1 >= file2);
}

TEST_F(File_test, compare_lt)
{
  auto file1{ alloc_file("ascii_1.txt") };
  auto file2{ alloc_file("ascii_2.txt") };
  ASSERT_TRUE(file1 < file2);
  ASSERT_FALSE(file1 > file2);
  ASSERT_TRUE(file1 != file2);
  ASSERT_FALSE(file1 == file2);
  ASSERT_TRUE(file1 <= file2);
  ASSERT_FALSE(file1 >= file2);
}

TEST_F(File_test, compare_gt)
{
  auto file1{ alloc_file("ascii_3.txt") };
  auto file2{ alloc_file("ascii_2.txt") };
  ASSERT_FALSE(file1 < file2);
  ASSERT_TRUE(file1 > file2);
  ASSERT_TRUE(file1 != file2);
  ASSERT_FALSE(file1 == file2);
  ASSERT_FALSE(file1 <= file2);
  ASSERT_TRUE(file1 >= file2);
}

TEST_F(File_test, copy_tmp_file)
{
  char const* fname{ "ascii_1.txt" };
  auto const  file1{ alloc_file(fname) };
  auto const  file2{ tmp_file(fname) };
  ASSERT_TRUE(file1 == file2);
  ASSERT_NE(file1.path(), file2.path());
}

TEST_F(File_test, move_tmp_file)
{
  char const* fname1{ "ascii_1.txt" };
  char const* fname2{ "moved_ascii_1.txt" };
  auto const  file1{ alloc_file(fname1) };
  auto        file2{ tmp_file(fname1) };
  ASSERT_TRUE(file1 == file2);
  ASSERT_NE(file1.path(), file2.path());
  ASSERT_TRUE(file2.move_to({ get_tmp_dir() / fname2 }) == rc::SUCCESS);
  ASSERT_TRUE(file1 == file2);
  ASSERT_NE(file1.path(), file2.path());
}

TEST_F(File_test, copy_and_move)
{
  char const* fname1{ "ascii_1.txt" };
  char const* fname2{ "moved_ascii_1.txt" };
  auto const  file1{ alloc_file(fname1) };
  auto        file2{ alloc_file(fname1) };
  fs::path    fpath1{ get_tmp_dir() / fname1 };
  fs::path    fpath2{ get_tmp_dir() / fname2 };
  WNDX_LOG(LL::DBUG, "fpath1: {}\n", fpath1.string());
  WNDX_LOG(LL::DBUG, "fpath2: {}\n", fpath2.string());
  ASSERT_NE(fpath1, fpath2);
  ASSERT_TRUE(file2.copy_to(fpath1) == rc::SUCCESS);
  ASSERT_TRUE(file1 == file2);
  ASSERT_EQ(file2.path(), fpath1);
  ASSERT_TRUE(file2.move_to(fpath2) == rc::SUCCESS);
  ASSERT_TRUE(file1 == file2);
  ASSERT_NE(file1.path(), file2.path());
  ASSERT_EQ(file2.path(), fpath2);
}

} // namespace wndx::mqlqd
