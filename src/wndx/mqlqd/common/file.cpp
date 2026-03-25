
#include "wndx/mqlqd/aliases.hpp" // IWYU pragma: keep

#include "wndx/mqlqd/file.hpp"

#include <fstream>


namespace wndx::mqlqd::file {

static constexpr std::string_view ctor{ "ctor - File instance" };

File::File(fs::path fpath, size_t sz) noexcept
    : m_block_size{ sz }
    , m_fpath{ std::move(fpath) }
{
  WNDX_LOG(LL::DBUG, "{} from file & size:\n\t{}\n", ctor, *this);
}

File::File(Finfo const& finfo, fs::path const& dpath) noexcept
    : m_block_size{ finfo.m_block_size }
    , m_fpath{ dpath / std::string(finfo.m_fname) } // concatenate path
{
  WNDX_LOG(LL::DBUG, "{} from Finfo & dir path:\n\t{}\n", ctor, *this);
}

void File::clean_heap() noexcept
{
  delete[] m_block;
  m_block = nullptr;
}

File::~File() noexcept { clean_heap(); }

[[nodiscard]] Finfo File::to_finfo() const noexcept
{
  Finfo finfo{};
  finfo.m_block_size = m_block_size;
  // fill the array - file name (element by element)
  auto fname{ m_fpath.filename().string() };
  for (size_t i = 0; i < fname.length(); ++i) {
    finfo.m_fname[i] = fname[i]; // NOLINT(*-constant-array-index)
  }
  // null terminator after the file name (important)
  finfo.m_fname[fname.length()] = '\0'; // NOLINT(*-constant-array-index)
  return finfo;
}

/// It is easy to use std::unique_ptr here for the mem_block
/// but maybe there is the benefit in proper using of the raw pointers?
/// It is easy to rewrite if requested!
[[nodiscard]] rc File::alloc() noexcept
{
  static constexpr auto fn{ "File::heap_alloc()" };
  try {
    m_block = { new char_type[m_block_size]{} };
    if (!m_block) {
      WNDX_LOG(LL::CRIT, "{} {}\n", fn, rc::MEMORY_BLOCK_EMPTY);
      return rc::MEMORY_BLOCK_EMPTY;
    }
    WNDX_LOG(LL::DBUG, "{}\n", fn);
  } catch (std::bad_alloc const& err) {
    WNDX_LOG(LL::ERRO, "{} {}\n{}\n", fn, rc::MEMORY_BAD_ALLOC, err.what());
    File::~File();
    return rc::MEMORY_BAD_ALLOC;
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    File::~File();
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

[[nodiscard]] rc File::write() const noexcept
{
  static constexpr auto fn{ "File::write()" };
  if (!m_block) {
    WNDX_LOG(LL::CRIT, "{} {}\n", fn, rc::MEMORY_BLOCK_EMPTY);
    return rc::MEMORY_BLOCK_EMPTY;
  }
  // open file for writing
  std::basic_fstream<char_type> ofs(m_fpath, openmode_w);
  // FIXME: these checks not valid for the not existing file!
  //        (file not yet written to the disk)
  // if (!ofs) {
  //   WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_CTOR_ERRO);
  //   return rc::FS_CTOR_ERRO;
  // }
  // if (ofs.fail()) {
  //   WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_OPEN_ERRO);
  //   return rc::FS_OPEN_ERRO;
  // }
  // if (!ofs.good()) {
  //   WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_IO_UNAVAILABLE);
  //   return rc::FS_IO_UNAVAILABLE;
  // }
  try {
    // NOTE: for loop is necessary for writing full contents of the binary file!
    for (size_t i = 0; i < m_block_size; i++) {
      ofs << m_block[i]; // NOLINT(*-pointer-arithmetic)
    }
    WNDX_LOG(LL::STAT, "{} SUCCESS:\n\t{}\n", fn, m_fpath);
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

[[nodiscard]] rc File::read() const noexcept
{
  static constexpr auto fn{ "File::fcontent()" };
  rc                    rc{ is_r(m_fpath) }; // return code
  if (rc != rc::SUCCESS) {
    return rc;
  } // open file for reading
  std::basic_fstream<char_type> ifs(m_fpath, openmode_r);
  // invariants & validation/safety checks
  if (!ifs) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_CTOR_ERRO);
    return rc::FS_CTOR_ERRO;
  }
  if (ifs.fail()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_OPEN_ERRO);
    return rc::FS_OPEN_ERRO;
  }
  if (!ifs.good()) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::FS_IO_UNAVAILABLE);
    return rc::FS_IO_UNAVAILABLE;
  }
  try { // we check on the ifs.fail() => will not be negative!
    auto const size{ ifs.tellg() };
    ifs.seekg(0, ifs.beg);
    ifs.read(m_block, size);
    WNDX_LOG(LL::INFO, "{} SUCCESS\n", fn);
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    return rc::CRIT_EX_UNHANDLED;
  }
  return rc::SUCCESS;
}

[[nodiscard]] rc File::alloc_and_read() noexcept
{
  static constexpr auto fn{ "File::read_to_block()" };
  rc                    rc = alloc();
  if (rc != rc::SUCCESS) {
    return rc;
  }
  try {
    rc = read();
    if (rc != rc::SUCCESS) {
      WNDX_LOG(LL::ERRO, "{} -> {}\n", fn, rc);
      return rc;
    }
    WNDX_LOG(LL::DBUG, "{} SUCCESS : {}\n", fn, m_fpath);
  } catch (std::exception const& err) {
    WNDX_LOG(LL::CRIT, "{} {}\n{}\n", fn, rc::CRIT_EX_UNHANDLED, err.what());
    File::~File();
    return rc::CRIT_EX_UNHANDLED;
  } catch (...) {
    WNDX_LOG(LL::CRIT, "{} occurred but was caught!\n{}\n",
             rc::CRIT_EX_ANONYMOUS, "THIS IS VERY BAD!");
    return rc::CRIT_EX_ANONYMOUS;
  }
  return rc::SUCCESS;
}

[[nodiscard]] rc File::copy_to(fs::path const& dst) noexcept
{
  static constexpr auto fn{ "File::copy_to()" };
  if (m_block == nullptr) {
    WNDX_LOG(LL::ERRO, "{} {}\n", fn, rc::MEMORY_BLOCK_EMPTY);
    return rc::MEMORY_BLOCK_EMPTY;
  }
  m_fpath = dst; // set new file path prior to the write to the disk.
  rc rc   = write();
  if (rc != rc::SUCCESS) {
    WNDX_LOG(LL::ERRO, "{} -> {}\n", fn, rc);
    return rc;
  }
  WNDX_LOG(LL::DBUG, "{} SUCCESS : {}\n", fn, m_fpath);
  return rc::SUCCESS;
}

[[nodiscard]] rc File::move_to(fs::path const& dst) noexcept
{
  static constexpr auto fn{ "File::move_to()" };
  fs::path const        old_fpath{ m_fpath };
  rc                    rc = copy_to(dst);
  if (rc != rc::SUCCESS) {
    WNDX_LOG(LL::ERRO, "{} -> {}\n", fn, rc);
    return rc;
  }
  std::error_code ec{};
  fs::remove(old_fpath, ec);
  if (ec) {
    WNDX_LOG(LL::ERRO, "[FAIL] fs::remove(old_fpath, ec) -> v:{} m:{}\n",
             ec.value(), ec.message());
  } else {
    WNDX_LOG(LL::DBUG, "[ OK ] fs::remove(\"{}\")\n", old_fpath);
  }
  WNDX_LOG(LL::DBUG, "{} SUCCESS : {}\n", fn, m_fpath);
  return rc::SUCCESS;
}

void File::print() const noexcept
{
  if (m_block == nullptr) {
    return;
  }
  // NOTE: for loop is necessary for printing full contents of the binary file!
  for (size_t i = 0; i < m_block_size; ++i) {
    fmt::print("{:c}", m_block[i]); // NOLINT(*-pointer-arithmetic)
  }
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
