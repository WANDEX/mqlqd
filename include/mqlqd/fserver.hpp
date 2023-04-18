#pragma once
/**
 * file server (receive side).
 */

#include "aliases.hpp"
// #include "config.hpp"

#include "structs_net_soc.hpp"


namespace mqlqd {

class Fserver final
{
public:
  // Fserver();
  Fserver() = delete; // XXX: or not delete...
  Fserver(Fserver &&) = default;
  Fserver(const Fserver &) = default;
  Fserver &operator=(Fserver &&) = default;
  Fserver &operator=(const Fserver &) = default;
  ~Fserver() = default;


  /**
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */

  /**
   * @brief man socket(2).
   *
   * On success, a file descriptor for the new socket is returned.
   * On error, -1 is returned, and errno is set to indicate the error.
   */
  [[nodiscard]] int
  create_socket();

  /**
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  create_connection();

  /**
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  wait_for_connections();

  /**
   * @brief @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  init();

private:
  [[nodiscard]] int
  fill_sockaddr_in();


private:
  // for the simple return code. (val chosen arbitrarily)
  int m_rc{ -42 };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  int m_fd{ -1 };

  // struct sockaddr

  // port_t m_port{ cfg::port };
  // addr_t m_addr{ cfg::addr };

  sockaddr_in m_sockaddr_in {};

  addrinfo    m_addrinfo    {};

  socklen_t   m_addrlen     {}; // XXX: part of addrinfo
  // TODO: probably better to rewrite later using addrinfo structure.
  //       If it make sense!

};

} // namespace mqlqd
