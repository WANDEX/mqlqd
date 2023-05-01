
#include "fclient.hpp"

#include "aliases.hpp"
#include "file.hpp"

#include <fmt/format.h>

#include <filesystem>
#include <vector>

extern "C" {

#include <arpa/inet.h>          // inet_pton(), inet_ntoa()

#include <netdb.h>              // XXX needed?

#include <sys/socket.h>
#include <sys/types.h>          // ssize_t

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)
#include <netinet/tcp.h>        // TCP protocol | tcp(7)

#include <unistd.h>             // | close(2).

// #include <stdlib.h>             // XXX needed?

} // extern "C"

namespace mqlqd {

Fclient::Fclient(addr_t const& addr, port_t const& port) noexcept
  : m_addr{ addr }, m_port{ port }
{
  log_g.msg(LL::DBUG, "INSIDE ctor Fclient()");
}

Fclient::~Fclient() noexcept
{
  log_g.msg(LL::DBUG, "INSIDE dtor ~Fclient()");
  // TODO: close_fd() | close(2) wrapper
  // close file descriptor. ref: close(2).
  if (m_fd > 0) {
    m_rc = close(m_fd);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd close()"); break;
    case  0: log_g.msg(LL::DBUG, "[ OK ] m_fd close()"); break;
    default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: m_fd close() -> {}", m_rc));
    }
    m_fd = -1;
  }
  log_g.msg(LL::DBUG, "END OF dtor ~Fclient()");
}

[[nodiscard]] int
Fclient::send_num_files_total(const size_t num_files_total)
{
  ssize_t nbytes = send(m_fd, &num_files_total, sizeof(num_files_total), 0);
  if (nbytes != sizeof(num_files_total)) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] to send() num_files_total, error occurred"); return -1;
    default: log_g.msg(LL::ERRO, fmt::format("[FAIL] sent num_files_total out of all bytes: {}/{}",
                                             nbytes, sizeof(num_files_total)));
    }
    return -2;
  }
  log_g.msg(LL::DBUG, fmt::format("[ OK ] send_num_files_total() : {}", num_files_total));
  return 0;
}

[[nodiscard]] int
Fclient::send_files_info(std::vector<file::mqlqd_finfo> const& vfinfo)
{
  // send num_files_total first => so that the server knows how many files to expect.
  m_rc = send_num_files_total(vfinfo.size());
  if (m_rc != 0) return m_rc;

  for (const auto& finfo : vfinfo) {
    m_rc = send_file_info(finfo);
    if (m_rc != 0) return m_rc;
  }
  log_g.msg(LL::INFO, "[ OK ] Sent info of the upcoming transfer of the files.");
  return 0;
}

[[nodiscard]] int
Fclient::send_file_info(file::mqlqd_finfo const& finfo)
{
  log_g.msg(LL::DBUG, fmt::format("INSIDE send_file_info() : {}", finfo));
  m_rc = send_loop<file::mqlqd_finfo>(m_fd, &finfo, sizeof(finfo));
  if (m_rc != 0) {
    log_g.msg(LL::ERRO, fmt::format("[FAIL] send_file_info() in send_loop() -> {} : {}", m_rc, finfo));
    return m_rc;
  }
  log_g.msg(LL::INFO, fmt::format("[ OK ] send_file_info() : {}", finfo));
  return 0;
}

[[nodiscard]] int
Fclient::send_files(std::vector<file::File> const& vfiles)
{
  for (const auto& file : vfiles) {
    m_rc = send_file(file);
    if (m_rc != 0) return m_rc;
  }
  log_g.msg(LL::NTFY, fmt::format("[ OK ] all files are sent: {}/{}",
                                  vfiles.size(), vfiles.size()));
  return 0;
}

[[nodiscard]] int
Fclient::send_file(file::File const& file)
{
  log_g.msg(LL::INFO, fmt::format("INSIDE send_file() : {}", file));
  m_rc = send_loop<char>(m_fd, file.m_block, file.m_block_size);
  if (m_rc != 0) {
    log_g.msg(LL::ERRO, fmt::format("[FAIL] send_file() in send_loop() -> {} : {}", m_rc, file));
    return m_rc;
  }
  log_g.msg(LL::STAT, fmt::format("[ OK ] send_file() : {}", file));
  return 0;
}

template <typename T>
[[nodiscard]] int
Fclient::send_loop(int fd, void const* buf, size_t len)
{
  T const* bufptr{ reinterpret_cast<T const*>(buf) };
  size_t  toread{ len };
  ssize_t nbytes{  -1 }; // nbytes sent || -1 - error val. ref: send(2).
  // loop till all bytes are sent or till the error.
  while ((nbytes = send(fd, bufptr, toread, 0)) > 0) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] send() error occurred"); return -1;
    case  0: log_g.msg(LL::CRIT, "[FAIL] send() -> 0 - nothing to send!"); return -2;
    default: log_g.msg(LL::DBUG, fmt::format("nbytes send_loop() :  {}", nbytes));
    }
    bufptr += nbytes; // next position to send into
    toread -= static_cast<size_t>(nbytes); // send less next time
  }
  log_g.msg(LL::DBUG, fmt::format("[ OK ] send_loop() finished."));
  return 0;
}

[[nodiscard]] int
Fclient::create_socket()
{
  // TODO: AF_UNSPEC everywhere instead of AF_INET?
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) {
    log_g.errnum(errno, "[FAIL] socket()");
    return -1;
  }
  log_g.msg(LL::DBUG, "[ OK ] socket()");
  return m_fd; // return file descriptor
}

[[nodiscard]] int
Fclient::create_connection()
{
  // NOTE: Not marked with __THROW
  m_rc = connect(m_fd, reinterpret_cast<const struct sockaddr *>(&m_sockaddr_in), m_addrlen);
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] connect()"); break;
  case  0: log_g.msg(LL::DBUG, "[ OK ] connect()"); break;
  default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: connect() -> {}", m_rc));
  }
  if (m_rc == 0) {
    log_g.msg(LL::NTFY, fmt::format("Connection established with: {}", inet_ntoa(m_sockaddr_in.sin_addr)));
  }
  return m_rc;
}


[[nodiscard]] int
Fclient::init()
{
  m_rc = create_socket();
  if (m_rc == -1) {
    log_g.msg(LL::ERRO, "[FAIL] in init() : create_socket()");
    return -1;
  }

  m_rc = fill_sockaddr_in();
  if (m_rc != 1) {
    log_g.msg(LL::ERRO, "[FAIL] in init() : fill_sockaddr_in()");
    return m_rc;
  }

  m_rc = create_connection();
  if (m_rc == -1) {
    log_g.msg(LL::ERRO, "[FAIL] in init() : create_connection()");
    return m_rc;
  }

  log_g.msg(LL::STAT, "[ OK ] init() - client initialized.");
  return 0;
}


[[nodiscard]] int
Fclient::fill_sockaddr_in()
{

  // m_addrinfo.ai_family  = AF_INET;
  // m_addrinfo.ai_addrlen = sizeof(m_sockaddr_in);

  // XXX: htons() - uint16_t
  // FIXME: check - does it work for the all u16 types?
  m_sockaddr_in.sin_family  = AF_INET;
  m_sockaddr_in.sin_port    = htons(m_port);
  m_addrlen = sizeof(m_sockaddr_in);

  // convert IPv4 and IPv6 addresses from text to binary form
  m_rc = inet_pton(AF_INET, m_addr.data(), &m_sockaddr_in.sin_addr);
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] inet_pton()"); break;
  case  0: log_g.msg(LL::WARN, "[FAIL] not valid network address in the specified address family!"); break;
  case  1: log_g.msg(LL::INFO, "[ OK ] network address was successfully converted"); break;
  default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: inet_pton() -> {}", m_rc));
  }
  return m_rc;
}

} // namespace mqlqd

