#pragma once
/**
 * file client (sending side / sending party).
 */

#include "aliases.hpp"
// #include "config.hpp"
#include "file.hpp"

#include <vector>

extern "C" {

// seems like it has most of the needed type definitions.
// #include <netdb.h>

// #include <sys/socket.h>
// #include <sys/types.h>

#include <netinet/in.h>         // Internet domain sockets | sockaddr(3type)

} // extern "C"


namespace mqlqd {

class Fclient final
{
public:
  Fclient() = default;
  // Fclient() = delete; // XXX: or not delete...
  Fclient(Fclient &&) = delete;
  Fclient(const Fclient &) = delete;
  Fclient &operator=(Fclient &&) = delete;
  Fclient &operator=(const Fclient &) = delete;
  ~Fclient();


  // TODO: pass port number into ctor
  // TODO: pass addr into ctor
  // explicit Fclient() noexcept;

  /**
   * @brief initialize everything & start on success of all underlying functions.
   *
   * @return 0 on success, else return fail code of the underlying functions.
   */
  [[nodiscard]] int
  init();

  /**
   * @brief description
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  send_files_info(std::vector<file::mqlqd_finfo> const& vfinfo);


  /**
   * @brief description
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  send_files(std::vector<file::File> const& vfiles);

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
   * @brief man connect(2).
   *
   * @return  0 on success.
   * @return -1 on error.
   */
  [[nodiscard]] int
  create_connection();

  /**
   * @brief man send(2).
   *
   * @return  N on success - the number of bytes sent.
   * @return  0 on success - when all bytes are sent (finish) (simplified).
   * @return -1 on error   - and errno is set to indicate the error.
   */
  [[nodiscard]] ssize_t
  send_loop(int fd, const char *buf, size_t len);

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
   * @brief description
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  send_file_info(file::mqlqd_finfo const& finfo);

  /**
   * @brief description
   *
   * @param  TODO
   * @return TODO
   *
   */
  [[nodiscard]] int
  send_file(file::File const& file);


private:
  // for the simple return code. (val chosen arbitrarily)
  int m_rc{ -42 };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  int m_fd{ -1 };

  // struct sockaddr;

  // port_t m_port{ cfg::port };
  // addr_t m_addr{ cfg::addr };

  // sockaddr_in m_sockaddr_in {};
  struct sockaddr_in m_sockaddr_in {};
  // struct sockaddr_in m_sockaddr_in;

  socklen_t   m_addrlen     {}; // XXX: part of addrinfo

  // TODO: probably better to rewrite later using addrinfo structure.
  //       If it make sense!
  // addrinfo    m_addrinfo    {};

};

} // namespace mqlqd
