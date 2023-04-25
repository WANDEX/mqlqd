
#include "fserver.hpp"

#include "aliases.hpp"
#include "file.hpp"

#include <fmt/format.h>
#include <iostream>

extern "C" {

#include <arpa/inet.h>          // inet_pton(), inet_ntoa()

#include <netdb.h>              // XXX needed?

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)
#include <netinet/tcp.h>        // TCP protocol | tcp(7)

#include <unistd.h>             // | close(2).

// #include <stdlib.h>             // XXX needed?

} // extern "C"

namespace mqlqd {

Fserver::Fserver(port_t const& port, fs::path const& storage_dir) noexcept
  : m_port{ port }, m_storage_dir{ storage_dir }
{
}

Fserver::~Fserver()
{
  // TODO: close file descriptors
  // TODO: close_fd() | close(2) wrapper
  // close file descriptors. ref: close(2).
  if (m_fd_con > 0) {
    m_rc = close(m_fd_con);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd_con close()"); break;
    case  0: log_g.msg(LL::DBUG, "[ OK ] m_fd_con close()"); break;
    default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: m_fd_con close() -> {}", m_rc));
    }
  }
  if (m_fd > 0) {
    m_rc = close(m_fd);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd close()"); break;
    case  0: log_g.msg(LL::DBUG, "[ OK ] m_fd close()"); break;
    default: log_g.msg(LL::CRIT, fmt::format("Unexpected return code: m_fd close() -> {}", m_rc));
    }
  }
}

[[nodiscard]] int
Fserver::recv_num_files_total()
{
  ssize_t nbytes = recv(m_fd_con, &m_num_files_total, sizeof(m_num_files_total), 0);
  if (nbytes != sizeof(m_num_files_total)) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] to recv() m_num_files_total, error occurred:"); return -1;
    default: log_g.msg(LL::ERRO, fmt::format("recv m_num_files_total out of all bytes: {}/{}",
                                             nbytes, sizeof(m_num_files_total)));
    }
    return -2;
  } else {
    log_g.msg(LL::DBUG, fmt::format("recv m_num_files_total: {} out of total bytes: {}/{}",
                                    m_num_files_total, nbytes, sizeof(m_num_files_total)));
  }
  return 0;
}

[[nodiscard]] int
Fserver::recv_files_info()
{
  log_g.msg(LL::DBUG, "Fserver::recv_files_info() entered into function.");
  m_rc = recv_num_files_total();
  if (m_rc != 0) return m_rc;
  // reserve in order to avoid potential reallocations later. (if many files)
  // m_vfinfo.reserve(m_num_files_total);
  m_vfiles.reserve(m_num_files_total);

  for (size_t i = 0; i < m_num_files_total; i++) {
    m_rc = recv_file_info(i);
    if (m_rc != 0) return m_rc;
  }
  return 0;
}

[[nodiscard]] int
Fserver::recv_file_info(const size_t i)
{
  log_g.msg(LL::DBUG, "Fserver::recv_file_info() entered into function.");

  // FIXME: reinspect docs & comments => (blindly copied from send)
  file::mqlqd_finfo finfo {};
  ssize_t nbytes{ -1 }; // nbytes sent || -1 - error val. ref: recv(2).
  ssize_t tbytes{ sizeof(finfo) }; // total bytes
  size_t  zbytes{ sizeof(finfo) }; // total bytes

  while ((nbytes = recv(m_fd_con, &finfo, zbytes, 0)) > 0) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] recv() error occurred"); return -1;
    case  0: log_g.msg(LL::WARN, "[DOUBTS] recv() -> 0 => orderly shutdown or what?"); break;
    default: log_g.msg(LL::DBUG, fmt::format("recv_file_info() bytes: {}", nbytes));
    }
    if (nbytes < 1) break; // -1 error || 0 orderly shutdown
    if (tbytes <= nbytes) break; // XXX: not sure
    // TODO: maybe i also should -= += here etc.
  }

  // log_g.msg(LL::DBUG, fmt::format("i: {}, finfo: {}", i, finfo));
  // m_vfinfo.emplace_back(finfo);

  // TODO: concatenate storage dir with file name
  m_vfiles.emplace_back(file::File{ fmt::format("{}/{}", m_storage_dir.c_str(), i), finfo });
  log_g.msg(LL::INFO, fmt::format("i: {}, fpath: {}", i, m_vfiles.at(i)));

  return 0;
}

[[nodiscard]] int
Fserver::recv_file(const size_t i)
{
  // reference variable to the needed file. (partially complete obj, which lacks file content).
  // here we recv the last missing element - contents of the file as the block of memory.
  file::File &file = m_vfiles.at(i);

  // TODO: it will be cool to make - "the small buffer optimization"
  //       => fixed size buffer on the stack for the small files.
  m_rc = file.heap_alloc();
  if (m_rc != 0) return m_rc;

  m_rc = recv_loop(m_fd_con, file.m_block, file.m_block_size);
  if (m_rc != 0) {
    log_g.msg(LL::ERRO, fmt::format("[FAIL] to recv file: {} : in recv_loop() -> {}",
                                    file.m_fpath.c_str(), m_rc));
    return m_rc;
  }
  log_g.msg(LL::STAT, fmt::format("[ OK ] recv file: {}", file.m_fpath.c_str()));

  // write file to the storage dir.
  m_rc = file.write();
  if (m_rc != 0) {
    return m_rc;
  }
  return 0;
}

[[nodiscard]] int
Fserver::recv_files()
{
  for (size_t i = 0; i < m_num_files_total; i++) {
    m_rc = recv_file(i);
    if (m_rc != 0) return m_rc;
  }
  log_g.msg(LL::NTFY, fmt::format("[ OK ] all files are received: {}/{}",
                                  m_num_files_total, m_num_files_total));
  return 0;
}

[[nodiscard]] int
Fserver::recv_loop(int fd, void *buf, size_t len)
{
  char   *bufptr{ reinterpret_cast<char *>(buf) };
  size_t  toread{ len };
  ssize_t nbytes{  -1 }; // nbytes recv || -1 - error val. ref: recv(2).
  // loop till all bytes are recv or till the error.
  while ((nbytes = recv(fd, bufptr, toread, 0)) > 0) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] recv() error occurred"); return -1;
    case  0: log_g.msg(LL::WARN, "[FAIL] recv() -> 0 - orderly shutdown"); return -2;
    default: log_g.msg(LL::DBUG, fmt::format("recv_loop() bytes: {}", nbytes));
    }
    bufptr += nbytes; // next position to read into
    toread -= static_cast<size_t>(nbytes); // read less next time
  }
  log_g.msg(LL::DBUG, fmt::format("[ OK ] in recv_loop()"));
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
  default: log_g.msg(LL::INFO, "New connected socket created.");
  }
  log_g.msg(LL::NTFY, fmt::format("Connection from: {}", inet_ntoa(m_sockaddr_in.sin_addr)));
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
  m_sockaddr_in.sin_port        = htons(m_port);
  m_sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  m_addrlen = sizeof(m_sockaddr_in);

  // 1 as success for consistency with
  // fclient fill_sockaddr_in() success return value.
  return 1;
}

} // namespace mqlqd

