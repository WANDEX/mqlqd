#pragma once
/**
 * file server (receiving side / host party).
 */

#include "aliases.hpp"
#include "file.hpp"

#include <vector>

extern "C" {

// seems like it has most of the needed type definitions.
// #include <netdb.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)

} // extern "C"


namespace mqlqd {

class Fserver final
{
public:
  Fserver() = delete;
  Fserver(Fserver &&) = delete;
  Fserver(const Fserver &) = delete;
  Fserver &operator=(Fserver &&) = delete;
  Fserver &operator=(const Fserver &) = delete;
  ~Fserver() noexcept;

  explicit Fserver(port_t const& port, fs::path const& storage_dir) noexcept;

  /**
   * @brief initialize everything & start on success of all underlying functions.
   *
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  init();

  /**
   * @brief recv info files structures, with the files information.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  recv_files_info();


  /**
   * @brief description
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  recv_files();

protected:

  /**
   * @brief man socket(2).
   *
   * @return file descriptor for the new socket (on success).
   * @return -1 on error.
   */
  [[nodiscard]] int
  create_socket();


  /**
   * @brief man bind(2).
   *
   * @return  0 on success.
   * @return -1 on error.
   */
  [[nodiscard]] int
  bind_socket();


  /**
   * @brief man listen(2).
   *
   * @return  0 on success.
   * @return -1 on error.
   */
  [[nodiscard]] int
  set_socket_in_listen_state();

  /**
   * @brief man accept(2).
   *
   * @return file descriptor for the new connected socket (on success).
   * @return -1 on error.
   */
  [[nodiscard]] int
  accept_connection();

protected:
  /****************************************************************************
   * following are the helper methods.
   */

  /**
   * @brief fill the sockaddr_in structure.
   */
  [[nodiscard]] int
  fill_sockaddr_in();

  /**
   * @brief recv num_files_total, so that the server knows how many to expect.
   *
   * @return TODO
   */
  [[nodiscard]] int
  recv_num_files_total();

  /**
   * @brief recv info file structure, with the file information.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  recv_file_info(const size_t i);


  /**
   * @brief recv file.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  recv_file(const size_t i);

  /**
   * @brief man recv(2).
   *
   * @return  0 on success - when all bytes are received (finish).
   * @return -1 on error   - and errno msg is logged to indicate the error.
   * @return -2 on recv() -> 0 - orderly shutdown etc. ref: recv(2).
   */
  template <typename T>
  [[nodiscard]] int
  recv_loop(int fd, void *buf, size_t len);


private:
  // initialized via explicit ctor
  const port_t m_port {};

  // for the simple return code. (val chosen arbitrarily)
  int m_rc{ -42 };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  int m_fd{ 0 };

  // new fd (connected socket) returned by the accept().
  // -1 is the accept() return value on error. ref: accept(2)
  int m_fd_con{ 0 };

  // The backlog defines the maximum length to which
  // the queue of pending connections may grow. ref: listen(2)
  const int m_backlog{ 5 }; // (default val chosen arbitrarily)

  size_t m_num_files_total{ 0 };

  // path to the storage dir. (storage for incoming files)
  const fs::path m_storage_dir; // initialized via explicit ctor

  socklen_t m_addrlen {}; // XXX: part of addrinfo

  struct sockaddr_in m_sockaddr_in {};

  std::vector<file::File>        m_vfiles;

};

} // namespace mqlqd

