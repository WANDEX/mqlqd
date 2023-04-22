
#include "fclient.hpp"

#include "aliases.hpp"
#include "config.hpp"           // for the: addr, port, uid
#include "file.hpp"


extern "C" {

#include <arpa/inet.h>          // inet_pton()

#include <netdb.h>              // XXX needed?

#include <sys/socket.h>
#include <sys/types.h>          // ssize_t

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)
#include <netinet/tcp.h>        // TCP protocol | tcp(7)

#include <unistd.h>             // | close(2).

// #include <stdlib.h>             // XXX needed?

} // extern "C"

namespace mqlqd {

// Fclient::Fclient()
// {
// }

Fclient::~Fclient()
{
  // TODO: close file descriptors
  // TODO: close_fd() | close(2) wrapper
  if (m_fd > 0) {
    // close file descriptor. ref: close(2).
    m_rc = close(m_fd);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd close()"); break;
    case  0: log_g.msg(LL::DBUG, "[ OK ] m_fd close()"); break;
    default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: m_fd close() -> {}", m_rc));
    }
  }
}

[[nodiscard]] int
Fclient::send_files_info(std::vector<file::mqlqd_finfo> const& vfinfo)
{
  // TODO
  for (const auto& finfo : vfinfo) {
    m_rc = send_file_info(finfo);

  }

  return 0;
}

[[nodiscard]] int
Fclient::send_file_info(file::mqlqd_finfo const& finfo)
{
  log_g.msg(LL::INFO, fmt::format("Fclient::send_file_info()\t{}", finfo));
  // TODO

  return 0;
}

[[nodiscard]] int
Fclient::send_files(std::vector<file::File> const& vfiles)
{
  // TODO
  for (const auto& file : vfiles) {
    m_rc = send_file(file);

  }

  return 0;
}

[[nodiscard]] int
Fclient::send_file(file::File const& file)
{
  log_g.msg(LL::INFO, fmt::format("Fclient::send_file()\t\t{}", file));
  // TODO

  return 0;
}

[[nodiscard]] int
Fclient::create_socket()
{
  // errno is set to indicate the error.
  // TODO: AF_UNSPEC everywhere instead of AF_INET?
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) {
    // XXX works? maybe i do not need to pass errno as the argument? -> integrate inside function?
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
  case  0: log_g.msg(LL::INFO, "connection/binding success"); break;
  default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: connect() -> {}", m_rc));
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

/*
 * following are the helper methods.
 */

[[nodiscard]] int
Fclient::fill_sockaddr_in()
{

  // m_addrinfo.ai_family  = AF_INET;
  // m_addrinfo.ai_addrlen = sizeof(m_sockaddr_in);

  // XXX: htons() - uint16_t
  // FIXME: check - does it work for the all u16 types?
  m_sockaddr_in.sin_family  = AF_INET;
  // FIXME support cmd option port
  m_sockaddr_in.sin_port    = htons(cfg::port);
  m_addrlen = sizeof(m_sockaddr_in);

  // FIXME: support cmd option addr
  // convert IPv4 and IPv6 addresses from text to binary form
  m_rc = inet_pton(AF_INET, cfg::addr.data(), &m_sockaddr_in.sin_addr);
#if 1 // XXX: I like this more! What do you like more?
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] inet_pton()"); break;
  case  0: log_g.msg(LL::ERRO, "Not valid network address in the specified address family!"); break;
  case  1: log_g.msg(LL::INFO, "Network address was successfully converted"); break;
  default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: inet_pton() -> {}", m_rc));
  }
#else
  if (m_rc == -1) {
    log_g.errnum(errno, "[FAIL] inet_pton()");
  } else if (m_rc == 0) {
    log_g.msg(LL::ERRO, "Not valid network address in the specified address family!");
  } else if (m_rc == 1) {
    log_g.msg(LL::INFO, "Network address was successfully converted");
  } else {
    log_g.msg(LL::CRIT, fmt::format("Unexpected return code: inet_pton() -> {}", m_rc));
  }
#endif
  return m_rc;
}

} // namespace mqlqd

