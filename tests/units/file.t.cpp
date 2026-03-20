#include "wndx/mqlqd/file.hpp"

#include <gtest/gtest.h>

#include <cstring> // memcpy
#include <filesystem>
#include <string>


namespace wndx::mqlqd {

class File_test : public ::testing::Test
{
public:
  /// \brief helper function to create File from the tests data files.
  static file::File alloc_file(std::string const& file_name = "ascii_1.txt")
  {
    fs::path dp;
    fs::path fp;
    EXPECT_NO_THROW({
      dp = "./data";
      fp = dp / file_name;
    });
    file::File File(fp, fs::file_size(fp));
    return File;
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
  ASSERT_EQ(file_name, std::string(finfo.m_fname));
}

TEST_F(File_test, ctor_finfo_path)
{
  auto const file_name{ "ascii_3.txt" }; // NOLINT(readability-qualified-auto)
  auto const len{ std::string(file_name).length() };
  auto const file{ alloc_file(file_name) };
  auto const finfo{ file.to_finfo() };
  ASSERT_EQ(file_name, std::string(finfo.m_fname));
  ASSERT_EQ(file.size(), fs::file_size(file.path()));

  file::Finfo finfo2;
  finfo2.m_block_size = file.size();
  finfo2.m_fname[len] = '\0';
  std::memcpy(&finfo2.m_fname, file_name, len);
  ASSERT_EQ(std::string(finfo.m_fname), std::string(finfo2.m_fname));
}

} // namespace wndx::mqlqd
