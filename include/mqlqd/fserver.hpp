#pragma once
/**
 * file server (receiving side / host party).
 */

#include "aliases.hpp"

extern "C" {

// seems like it has most of the needed type definitions.
// #include <netdb.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)

} // extern "C"


namespace mqlqd {

class Fserver final
{
public:
  Fserver() = default;
  // Fserver() = delete; // XXX: or not delete...
  Fserver(Fserver &&) = delete;
  Fserver(const Fserver &) = delete;
  Fserver &operator=(Fserver &&) = delete;
  Fserver &operator=(const Fserver &) = delete;
  ~Fserver() = default;


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

  /**
   * @brief initialize everything & start on success of all underlying functions.
   *
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  init();

/*
 * following are the helper methods.
 */

private:
  [[nodiscard]] int
  fill_sockaddr_in();


private:
  // for the simple return code. (val chosen arbitrarily)
  int m_rc{ -42 };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  int m_fd{ -1 };

  // new fd (connected socket) returned by the accept().
  // -1 is the accept() return value on error. ref: accept(2)
  int m_fd_con{ -1 };

  // The backlog defines the maximum length to which
  // the queue of pending connections may grow. ref: listen(2)
  const int m_backlog{ 5 }; // (default val chosen arbitrarily)

  struct sockaddr_in m_sockaddr_in {};

  socklen_t   m_addrlen     {}; // XXX: part of addrinfo

};

} // namespace mqlqd

