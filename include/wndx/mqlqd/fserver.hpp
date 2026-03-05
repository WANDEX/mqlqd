#pragma once
/**
 * file server (receiving side / host party).
 */

#include "aliases.hpp"

#include "file.hpp"

#include <vector>

extern "C" {

#include <netinet/in.h> // Internet domain sockets | sockaddr(3type)

} // extern "C"


namespace wndx::mqlqd {

class Fserver final
{
public:
  Fserver()                          = delete;
  Fserver(Fserver&&)                 = delete;
  Fserver(Fserver const&)            = delete;
  Fserver& operator=(Fserver&&)      = delete;
  Fserver& operator=(Fserver const&) = delete;
  ~Fserver() noexcept;

  explicit Fserver(port_t port, fs::path storage_dir) noexcept;

  /**
   * @brief initialize everything & start on success of all underlying
   * functions.
   *
   * @brief @return 0 on success, else return fail code of the underlying
   * functions.
   */
  [[nodiscard]] rc init();

  /**
   * @brief recv info files structures, with the files information.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] rc recv_files_info();


  /**
   * @brief description
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] rc recv_files();

protected:
  /**
   * @brief man socket(2).
   *
   * @return file descriptor for the new socket (on success).
   * @return -1 on error.
   */
  [[nodiscard]] int create_socket();


  /**
   * @brief man bind(2).
   *
   * @return  0 on success.
   * @return -1 on error.
   */
  [[nodiscard]] int bind_socket();


  /**
   * @brief man listen(2).
   *
   * @return  0 on success.
   * @return -1 on error.
   */
  [[nodiscard]] int set_socket_in_listen_state();

  /**
   * @brief man accept(2).
   *
   * @return file descriptor for the new connected socket (on success).
   * @return -1 on error.
   */
  [[nodiscard]] int accept_connection();

  /****************************************************************************
   * following are the helper methods.
   */

  [[nodiscard]] std::string host_addr_ipv4() const noexcept;

  /**
   * @brief make unique sub-dirs inside the root storage dir.
   * (to differentiate the source of the files and store them separately).
   *
   * @return  0 on success - when all sub-dirs successfully created.
   */
  [[nodiscard]] rc mkdir_sub_storage();

  /**
   * @brief fill the sockaddr_in structure.
   */
  [[nodiscard]] int fill_sockaddr_in();

  /**
   * @brief recv num_files_total, so that the server knows how many to expect.
   *
   * @return TODO
   */
  [[nodiscard]] int recv_num_files_total();

  /**
   * @brief recv info file structure, with the file information.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int recv_file_info(size_t const i);


  /**
   * @brief recv file.
   *
   * @param  TODO
   * @return TODO
   */
  [[nodiscard]] int recv_file(size_t const i);

  /**
   * @brief man recv(2).
   *
   * @return  0 on success - when all bytes are received (finish).
   * @return -1 on error   - and errno msg is logged to indicate the error.
   * @return -2 on recv() -> 0 - orderly shutdown etc. ref: recv(2).
   */
  template <typename T = file::File::char_type>
  [[nodiscard]] int recv_loop(int fd, void* buf, size_t len);


private:
  // initialized via explicit ctor
  port_t const m_port{};

  // path to the storage dir. (root of the storage)
  fs::path const m_storage_dir;

  // sub-storage inside the storage (for incoming files)
  // see: mkdir_sub_storage() - overrides this variable.
  fs::path m_storage_dir_sub{ m_storage_dir };

  // The backlog defines the maximum length to which
  // the queue of pending connections may grow. ref: listen(2)
  int const m_backlog{ 1 };

  // reusable for the POSIX return codes
  int m_rc{ static_cast<int>(rc::INIT) };

  // file descriptor returned by the socket().
  // -1 is the socket() return value on error. ref: socket(2)
  int m_fd{ -1 };

  // new fd (connected socket) returned by the accept().
  // -1 is the accept() return value on error. ref: accept(2)
  int m_fd_con{ -1 };

  size_t m_num_files_total{ 0 };

  socklen_t m_addrlen{};

  struct sockaddr_in m_sockaddr_in{};

  std::vector<file::File> m_vfiles;
};

} // namespace wndx::mqlqd
