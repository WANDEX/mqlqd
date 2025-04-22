
#include "mqlqd/fserver.hpp"

#include "mqlqd/aliases.hpp"
#include "mqlqd/file.hpp"

#include <fmt/format.h>

extern "C" {

#include <arpa/inet.h>          // inet_pton(), inet_ntoa()

#include <netdb.h>              // XXX needed?

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)
#include <netinet/tcp.h>        // TCP protocol | tcp(7)

#include <unistd.h>             // | close(2).

} // extern "C"

namespace mqlqd {

Fserver::Fserver(port_t const& port, fs::path const& storage_dir) noexcept
  : m_port{ port }, m_storage_dir{ storage_dir }
{
  WNDX_LOG(LL::DBUG, "INSIDE ctor Fserver()\n", "");
}

Fserver::~Fserver() noexcept
{
  WNDX_LOG(LL::DBUG, "INSIDE dtor ~Fserver()\n", "");
  // TODO: close_fd() | close(2) wrapper
  // close file descriptors. ref: close(2).
  if (m_fd_con > 0) {
    m_rc = close(m_fd_con);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd_con close()"); break;
    case  0:  WNDX_LOG(LL::DBUG, "[ OK ] m_fd_con close()\n", ""); break;
    default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: m_fd_con close() -> {}\n", m_rc);
    }
    m_fd_con = -1;
  }
  if (m_fd > 0) {
    m_rc = close(m_fd);
    switch (m_rc) {
    case -1: log_g.errnum(errno, "[FAIL] m_fd close()"); break;
    case  0:  WNDX_LOG(LL::DBUG, "[ OK ] m_fd close()\n", ""); break;
    default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: m_fd close() -> {}\n", m_rc);
    }
    m_fd = -1;
  }
  /*
   * NOTE: extra new line to split log messages
   * between the old & new class instance by the empty line.
   * For the daemon mode -> file server (in the infinite loop).
   */
  WNDX_LOG(LL::DBUG, "END OF dtor ~Fserver()\n\n", "");
}

[[nodiscard]] int
Fserver::recv_num_files_total()
{
  ssize_t nbytes = recv(m_fd_con, &m_num_files_total, sizeof(m_num_files_total), 0);
  if (nbytes != sizeof(m_num_files_total)) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] to recv() m_num_files_total, error occurred:"); return -1;
    default:  WNDX_LOG(LL::ERRO, "[FAIL] recv m_num_files_total out of all bytes: {}/{}\n",
                                 nbytes, sizeof(m_num_files_total));
    }
    return -2;
  }
  WNDX_LOG(LL::DBUG, "[ OK ] recv_num_files_total() : {}\n", m_num_files_total);
  return 0;
}

[[nodiscard]] int
Fserver::recv_files_info()
{
  // recv m_num_files_total first => so that the server knows how many files to expect.
  m_rc = recv_num_files_total();
  if (m_rc != 0) return m_rc;
  // reserve in order to avoid potential reallocations later. (if many files)
  m_vfiles.reserve(m_num_files_total);

  for (size_t i = 0; i < m_num_files_total; i++) {
    m_rc = recv_file_info(i);
    if (m_rc != 0) return m_rc;
  }
  WNDX_LOG(LL::INFO, "[ OK ] received info of the upcoming transfer of the files\n", "");
  return 0;
}

[[nodiscard]] int
Fserver::recv_file_info(const size_t i)
{
  WNDX_LOG(LL::DBUG, "INSIDE recv_file_info() : {}\n", i);
  file::mqlqd_finfo finfo {};
  m_rc = recv_loop<file::mqlqd_finfo>(m_fd_con, &finfo, sizeof(finfo));
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] recv_file_info() in recv_loop() -> {} : {}\n", m_rc, finfo);
    return m_rc;
  }
  WNDX_LOG(LL::INFO, "[ OK ] recv_file_info() : {}\n", finfo);
  // construct file object from the file info structure.
  m_vfiles.emplace_back(file::File{ finfo, m_storage_dir_sub });
  // WNDX_LOG(LL::DBUG, "\ti - {} : {}\n", i, m_vfiles.at(i)); // XXX: too verbose
  return 0;
}

[[nodiscard]] int
Fserver::recv_files()
{
  for (size_t i = 0; i < m_num_files_total; i++) {
    m_rc = recv_file(i);
    if (m_rc != 0) return m_rc;
  }
  WNDX_LOG(LL::NTFY, "[ OK ] all files are received: {}/{}\n",
                     m_num_files_total, m_num_files_total);
  return 0;
}

[[nodiscard]] int
Fserver::recv_file(const size_t i)
{
  // reference variable to the needed file. (partially complete obj, which lacks file content).
  // here we recv the last missing element - contents of the file as the block of memory.
  file::File &file = m_vfiles.at(i);
  WNDX_LOG(LL::INFO, "INSIDE recv_file() : {}\n", file);

  // TODO: it will be cool to make - "the small buffer optimization"
  //       => fixed size buffer on the stack for the small files.
  m_rc = file.heap_alloc();
  if (m_rc != 0) return m_rc;

  m_rc = recv_loop(m_fd_con, file.m_block, file.m_block_size);
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] recv_file() in recv_loop() -> {} : {}\n", m_rc, file);
    return m_rc;
  }
  WNDX_LOG(LL::STAT, "[ OK ] recv_file() : {}\n", file);

  // write file to the storage dir.
  m_rc = file.write();
  if (m_rc != 0) {
    return m_rc;
  }
  return 0;
}

template <typename T>
[[nodiscard]] int
Fserver::recv_loop(int fd, void *buf, size_t len)
{
  T      *bufptr{ reinterpret_cast<T *>(buf) };
  size_t  toread{ len };
  ssize_t nbytes{  -1 }; // nbytes recv || -1 - error val. ref: recv(2).
  // loop till all bytes are recv or till the error.
  while ((nbytes = recv(fd, bufptr, toread, 0)) > 0) {
    switch (nbytes) {
    case -1: log_g.errnum(errno, "[FAIL] recv() error occurred"); return -1;
    case  0:  WNDX_LOG(LL::WARN, "[FAIL] recv() -> 0 - orderly shutdown!\n", ""); return -2;
    default:  WNDX_LOG(LL::DBUG, "nbytes recv_loop() :  {}\n", nbytes);
    }
    bufptr += nbytes; // next position to read into
    toread -= static_cast<size_t>(nbytes); // read less next time
  }
  WNDX_LOG(LL::DBUG, "[ OK ] recv_loop() finished\n", "");
  return 0;
}

[[nodiscard]] int
Fserver::create_socket()
{
  // errno is set to indicate the error.
  // TODO: AF_UNSPEC everywhere instead of AF_INET?
  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) {
    log_g.errnum(errno, "[FAIL] socket()");
    return -1;
  }
  WNDX_LOG(LL::DBUG, "[ OK ] socket()\n", "");
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
  WNDX_LOG(LL::DBUG, "[ OK ] bind()\n", "");
  return m_rc; // return the return code -> 0 - success.
}

[[nodiscard]] int
Fserver::set_socket_in_listen_state()
{
  // Mark socket as a passive socket, that is, as a socket that
  // will be used to accept incoming connection requests using accept(2).
  m_rc = listen(m_fd, m_backlog);
  switch (m_rc) {
  case -1: log_g.errnum(errno, "[FAIL] listen()"); break;
  case  0:  WNDX_LOG(LL::INFO, "[ OK ] marked socket to accept incoming connection requests\n", ""); break;
  default:  WNDX_LOG(LL::CRIT, "UNEXPECTED return code: listen() -> {}\n", m_rc);
  }
  if (m_fd_con == 0) {
    WNDX_LOG(LL::NTFY, "backlog: {} (Max queue len of pending connections)\n", m_backlog);
  }
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
  case  0:  WNDX_LOG(LL::WARN, "[DOUBT] accept() -> 0 ???\n", ""); break;
  default:  WNDX_LOG(LL::DBUG, "[ OK ] accept() - new connected socket created\n", "");
  }
  if (m_fd_con > 0) {
    WNDX_LOG(LL::NTFY, "accepted connection from: {}\n", inet_ntoa(m_sockaddr_in.sin_addr));
  }
  return m_fd_con;
}

[[nodiscard]] int
Fserver::init()
{
  m_rc = create_socket();
  if (m_rc == -1) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : create_socket()\n", "");
    return -1;
  }

  m_rc = fill_sockaddr_in();
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : fill_sockaddr_in()\n", "");
    return m_rc;
  }

  m_rc = bind_socket();
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : bind_socket()\n", "");
    return m_rc;
  }

  m_rc = set_socket_in_listen_state();
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : set_socket_in_listen_state()\n", "");
    return m_rc;
  }

  m_rc = accept_connection();
  if (m_rc < 1) { // a non negative integer on success (XXX: excluding 0 i guess... right?)
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : accept_connection()\n", "");
    return m_rc;
  }

  m_rc = mkdir_sub_storage();
  if (m_rc != 0) {
    WNDX_LOG(LL::ERRO, "[FAIL] in init() : mkdir_sub_storage()\n", "");
    return m_rc;
  }

  WNDX_LOG(LL::STAT, "[ OK ] init() - server initialized\n", "");
  return 0;
}

[[nodiscard]] int
Fserver::fill_sockaddr_in()
{
  // m_addrinfo.ai_family  = AF_INET;
  // m_addrinfo.ai_addrlen = sizeof(m_sockaddr_in);

  // XXX: htons() - uint16_t
  // FIXME: check - does it work for the all u16 types?
  m_sockaddr_in.sin_family      = AF_INET;
  m_sockaddr_in.sin_port        = htons(m_port);
  m_sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  m_addrlen = sizeof(m_sockaddr_in);

  return 0;
}

[[nodiscard]] int
Fserver::mkdir_sub_storage()
{
  fs::path new_sub_storage_dir{ m_storage_dir_sub };
  new_sub_storage_dir += '/';
  new_sub_storage_dir += inet_ntoa(m_sockaddr_in.sin_addr); // in addr subdir
  // TODO: MAC/UID additionally.

  int rc = file::mkdir(new_sub_storage_dir, fs::perms::owner_all);
  if (rc != 0) {
    // m_storage_dir_sub = m_storage_dir; // XXX: or needed?
    return rc;
  }
  m_storage_dir_sub = new_sub_storage_dir;
  return 0;
}

} // namespace mqlqd

