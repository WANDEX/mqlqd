
#include "mqlqd/fclient.hpp"

#include "mqlqd/aliases.hpp"
#include "mqlqd/file.hpp"

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
  WNDX_LOG(LL::DBUG, "INSIDE ctor Fclient()\n");
}

Fclient::~Fclient() noexcept
{
  WNDX_LOG(LL::DBUG, "INSIDE dtor ~Fclient()\n");
  // TODO: close_fd() | close(2) wrapper
  // close file descriptor. ref: close(2).
  if (m_fd > 0) {
    m_rc = close(m_fd);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd close()"); break;
    case  0:  WNDX_LOG(LL::DBUG, "[ OK ] m_fd close()\n"); break;
    default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: m_fd close() -> {}\n", m_rc);
    }
    m_fd = -1;
  }
  WNDX_LOG(LL::DBUG, "END OF dtor ~Fclient()\n");
}

[[nodiscard]] int
Fclient::send_num_files_total(const size_t num_files_total)
{
  ssize_t nbytes = send(m_fd, &num_files_total, sizeof(num_files_total), 0);
  if (nbytes != sizeof(num_files_total)) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] to send() num_files_total, error occurred"); return -1;
    default:  WNDX_LOG(LL::ERRO, "[FAIL] sent num_files_total out of all bytes: {}/{}\n",
                                 nbytes, sizeof(num_files_total));
    }
    return -2;
  }
  WNDX_LOG(LL::DBUG, "[ OK ] send_num_files_total() : {}\n", num_files_total);
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
  WNDX_LOG(LL::INFO, "[ OK ] sent info of the upcoming transfer of the files.\n");
  return 0;
}

[[nodiscard]] int
Fclient::send_file_info(file::mqlqd_finfo const& finfo)
{
  WNDX_LOG(LL::DBUG, "INSIDE send_file_info() : {}\n", finfo);
  m_rc = send_loop<file::mqlqd_finfo>(m_fd, &finfo, sizeof(finfo));
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] send_file_info() in send_loop() -> {} : {}\n", m_rc, finfo);
    return m_rc;
  }
  WNDX_LOG(LL::INFO, "[ OK ] send_file_info() : {}\n", finfo);
  return 0;
}

[[nodiscard]] int
Fclient::send_files(std::vector<file::File> const& vfiles)
{
  for (const auto& file : vfiles) {
    m_rc = send_file(file);
    if (m_rc != 0) return m_rc;
  }
  WNDX_LOG(LL::NTFY, "[ OK ] all files are sent: {}/{}\n",
                     vfiles.size(), vfiles.size());
  return 0;
}

[[nodiscard]] int
Fclient::send_file(file::File const& file)
{
  WNDX_LOG(LL::INFO, "INSIDE send_file() : {}\n", file);
  m_rc = send_loop(m_fd, file.m_block, file.m_block_size);
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] send_file() in send_loop() -> {} : {}\n", m_rc, file);
    return m_rc;
  }
  WNDX_LOG(LL::STAT, "[ OK ] send_file() : {}\n", file);
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
    case  0:  WNDX_LOG(LL::CRIT, "[FAIL] send() -> 0 - nothing to send!\n"); return -2;
    default:  WNDX_LOG(LL::DBUG, "nbytes send_loop() :  {}\n", nbytes);
    }
    bufptr += nbytes; // next position to send into
    toread -= static_cast<size_t>(nbytes); // send less next time
  }
  WNDX_LOG(LL::DBUG, "[ OK ] send_loop() finished\n");
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
  WNDX_LOG(LL::DBUG, "[ OK ] socket()\n");
  return m_fd; // return file descriptor
}

[[nodiscard]] int
Fclient::create_connection()
{
  // NOTE: Not marked with __THROW
  m_rc = connect(m_fd, reinterpret_cast<const struct sockaddr *>(&m_sockaddr_in), m_addrlen);
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] connect()"); break;
  case  0:  WNDX_LOG(LL::DBUG, "[ OK ] connect()\n"); break;
  default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: connect() -> {}\n", m_rc);
  }
  if (m_rc == 0) {
    WNDX_LOG(LL::NTFY, "connection established with: {}\n", inet_ntoa(m_sockaddr_in.sin_addr));
  }
  return m_rc;
}


[[nodiscard]] int
Fclient::init()
{
  m_rc = create_socket();
  if (m_rc == -1) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : create_socket()\n");
    return -1;
  }

  m_rc = fill_sockaddr_in();
  if (m_rc != 1) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : fill_sockaddr_in()\n");
    return m_rc;
  }

  m_rc = create_connection();
  if (m_rc == -1) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : create_connection()\n");
    return m_rc;
  }

  WNDX_LOG(LL::STAT, "[ OK ] init() - client initialized.\n");
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
  case  0:  WNDX_LOG(LL::WARN, "[FAIL] not valid network address in the specified address family!\n"); break;
  case  1:  WNDX_LOG(LL::INFO, "[ OK ] network address was successfully converted\n"); break;
  default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: inet_pton() -> {}\n", m_rc);
  }
  return m_rc;
}

} // namespace mqlqd

