
#include "fserver.hpp"

#include "aliases.hpp"
#include "config.hpp"           // for the: addr, port, uid
#include "file.hpp"


extern "C" {

#include <arpa/inet.h>          // inet_pton()

#include <netdb.h>              // XXX needed?

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)
#include <netinet/tcp.h>        // TCP protocol | tcp(7)

#include <unistd.h>             // | close(2).

// #include <stdlib.h>             // XXX needed?

} // extern "C"

namespace mqlqd {

Fserver::Fserver(fs::path const& storage_dir) noexcept
  : m_storage_dir{ storage_dir }
{
}

Fserver::~Fserver()
{
  // TODO: close file descriptors
  // TODO: close_fd() | close(2) wrapper
  if (m_fd_con > 0) {
    // close file descriptor. ref: close(2).
    m_rc = close(m_fd_con);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] fd close()"); break;
    case  0: log_g.msg(LL::DBUG, "[ OK ] fd close()"); break;
    default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: fd close() -> {}", m_rc));
    }
  }
}


[[nodiscard]] int
Fserver::recv_files_info()
{
  // TODO

  return 0;
}


[[nodiscard]] int
Fserver::recv_files()
{
  // TODO

  return 0;
}

[[nodiscard]] int
Fserver::create_socket()
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
Fserver::bind_socket()
{
  // ref: bind(2) - for the explanation about the cast etc.
  m_rc = bind(m_fd, reinterpret_cast<const struct sockaddr *>(&m_sockaddr_in), m_addrlen);
  if (m_rc == -1) {
    log_g.errnum(errno, "[FAIL] bind()");
    return -1;
  }
  log_g.msg(LL::DBUG, "[ OK ] bind()");
  return m_rc; // return the return code (written like this for the consistency)
}

[[nodiscard]] int
Fserver::set_socket_in_listen_state()
{
  // Mark socket as a passive socket, that is, as a socket that
  // will be used to accept incoming connection requests using accept(2).
  m_rc = listen(m_fd, m_backlog);
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] listen()"); break;
  case  0: log_g.msg(LL::INFO, "Marked socket to accept incoming connection requests."); break;
  default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: listen() -> {}", m_rc));
  }
  log_g.msg(LL::NTFY, fmt::format("Backlog: {} (Max queue len of pending connections).", m_backlog));
  return m_rc;
}

[[nodiscard]] int
Fserver::accept_connection()
{
  // casts are the necessity! ref: bind(2), accept(2)
  m_fd_con = accept(m_fd, reinterpret_cast<struct sockaddr *>(&m_sockaddr_in),
                          reinterpret_cast<socklen_t *>(&m_addrlen));
  switch (m_fd_con) {
  case -1: log_g.errnum(errno, "[FAIL] accept()"); break;
  default: log_g.msg(LL::INFO, "New connected socket created."); break;
  }
  return m_fd_con;
}

[[nodiscard]] int
Fserver::init()
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

  m_rc = bind_socket();
  if (m_rc != 0) {
    log_g.msg(LL::ERRO, "[FAIL] in init() : bind_socket()");
    return m_rc;
  }

  m_rc = set_socket_in_listen_state();
  if (m_rc != 0) {
    log_g.msg(LL::ERRO, "[FAIL] in init() : set_socket_in_listen_state()");
    return m_rc;
  }

  m_rc = accept_connection();
  if (m_rc < 1) { // a non negative integer on success (XXX: excluding 0 i guess... right?)
    log_g.msg(LL::ERRO, "[FAIL] in init() : accept_connection()");
    return m_rc;
  }

  log_g.msg(LL::STAT, "[ OK ] init() - server initialized.");
  return 0;
}

/*
 * following are the helper methods.
 */

[[nodiscard]] int
Fserver::fill_sockaddr_in()
{
  // m_addrinfo.ai_family  = AF_INET;
  // m_addrinfo.ai_addrlen = sizeof(m_sockaddr_in);

  // XXX: htons() - uint16_t
  // FIXME: check - does it work for the all u16 types?
  // TODO: support cmd option port
  m_sockaddr_in.sin_family      = AF_INET;
  m_sockaddr_in.sin_port        = htons(cfg::port);
  m_sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  m_addrlen = sizeof(m_sockaddr_in);

  // 1 as success for consistency with
  // fclient fill_sockaddr_in() success return value.
  return 1;
}

} // namespace mqlqd

