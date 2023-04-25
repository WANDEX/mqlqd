#pragma once
/**
 * file client (sending side / sending party).
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

class Fclient final
{
public:
  Fclient() = delete;
  Fclient(Fclient &&) = delete;
  Fclient(const Fclient &) = delete;
  Fclient &operator=(Fclient &&) = delete;
  Fclient &operator=(const Fclient &) = delete;
  ~Fclient() noexcept;

  explicit Fclient(addr_t const& addr, port_t const& port) noexcept;

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
   * @brief send num_files_total, so that the server knows how many to expect.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int
  send_num_files_total(const size_t num_files_total);

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

  /**
   * @brief man send(2).
   *
   * @return  0 on success - when all bytes are sent (finish).
   * @return -1 on error   - and errno msg is logged to indicate the error.
   * @return -2 on send() -> 0 - nothing to send etc.
   */
  template <typename T>
  [[nodiscard]] int
  send_loop(int fd, void const* buf, size_t len);


private:
  // initialized via explicit ctor
  const addr_t m_addr {};
  const port_t m_port {};

  // for the simple return code. (val chosen arbitrarily)
  int m_rc{ -42 };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  // => XXX: lets try 0 as default.
  int m_fd{ 0 };

  socklen_t m_addrlen {}; // XXX: part of addrinfo

  struct sockaddr_in m_sockaddr_in {};

  // TODO: probably better to rewrite later using addrinfo structure.
  //       If it make sense!
  // addrinfo    m_addrinfo    {};

};

} // namespace mqlqd

