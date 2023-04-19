#pragma once
/**
 * file client (sender side).
 * FIXME: seems like i flipped sides...
 */

#include "aliases.hpp"

namespace /* (anonymous) */ {
extern "C" {

#include <netdb.h>

} // extern "C"
} // (anonymous) [internal_linkage]


namespace mqlqd {

class Fclient final
{
public:
  // Fclient();
  Fclient() = delete; // XXX: or not delete...
  Fclient(Fclient &&) = default;
  Fclient(const Fclient &) = default;
  Fclient &operator=(Fclient &&) = default;
  Fclient &operator=(const Fclient &) = default;
  ~Fclient() = default;


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
   * @return TODO
   * @return -1 on error.
   */
  [[nodiscard]] int
  accept_connection();

  /**
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

  struct sockaddr_in m_sockaddr_in {};

  socklen_t   m_addrlen     {}; // XXX: part of addrinfo

};

} // namespace mqlqd

