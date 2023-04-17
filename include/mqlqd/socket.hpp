#pragma once
/**
 * Abstraction layer on top of the Linux socket API.
 * (wrapper around the user API C language functions).
 * Socket Base class common to the both targets:
 * client (app) & server (daemon).
 * 'https://isocpp.org/wiki/faq/mixing-c-and-cpp'
 * TODO: !!! UNFINISHED !!!
 */

#include "aliases.hpp"
#include "config.hpp"


namespace mqlqd {
// TODO maybe also nest this into the anonymous namespace for the internal linkage?
extern "C" {

/**
 * // respective manual pages (docs):
 * // getaddrinfo(3), socket(2), bind(2), listen(2), accept(2)
 * // send(2), recv(2), close(2), socket(7), etc.
 * // $ man 2 socket
 * // $ man 3 sockaddr
 *
 * // sockaddr, sockaddr_storage, socklen_t, sa_family_t
 * #include <sys/socket.h>
 *
 * // Internet domain sockets:
 * #include <netinet/in.h>
 *
 * // together they have etc.:
 * // sockaddr,    sockaddr_storage,
 * // sockaddr_in, sockaddr_in6,
 * // sockaddr_un, socklen_t,
 * // in_addr,     in6_addr,
 * // in_addr_t,   in_port_t
 *
 * // UNIX domain sockets: sockaddr_un
 * #include <sys/un.h>
 *
 */

/*
 * forward declare required typedefs:
 */

// TODO

/*
 * forward declare Linux socket API C functions:
 */

// TODO

/*
 * #include <sys/types.h>
 * #include <sys/socket.h>
 * #include <netdb.h>
 * declared in those headers:
 */

/*
int   getaddrinfo(const char *restrict node,
                  const char *restrict service,
                  const struct addrinfo *restrict hints,
                  struct addrinfo **restrict res);

void freeaddrinfo(struct addrinfo *res);
*/

/*
 * #include <sys/socket.h>
 * declared in this header:
 */


int socket(int domain, int type, int protocol);
int   bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);

/*
 * @brief look into $ man 2 accept .
 *
 * original function signature looks like this!
 * int accept(int sockfd, struct sockaddr *_Nullable restrict addr,
 *            socklen_t *_Nullable restrict addrlen);
 * => FIXME
 */
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

}

class SocketBase
{
public:
  SocketBase();
  SocketBase(SocketBase &&) = delete;
  SocketBase(const SocketBase &) = delete;
  SocketBase &operator=(SocketBase &&) = delete;
  SocketBase &operator=(const SocketBase &) = delete;
  virtual ~SocketBase() noexcept;

  [[nodiscard]] int
  socket(int domain, int type, int protocol)
  {
    return mqlqd::socket(domain, type, protocol);
  }

  [[nodiscard]] int
  bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
  {
    return mqlqd::bind(sockfd, reinterpret_cast<struct sockaddr *>(&addr), addrlen);
  }

  [[nodiscard]] int
  listen(int sockfd, int backlog)
  {
    return mqlqd::listen(sockfd, backlog);
  }

  // [[nodiscard]] int
  // accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);

  // FIXME: the method signature differ from the original function!
  [[nodiscard]] int
  accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
  {
    return mqlqd::accept(sockfd,
        reinterpret_cast<struct sockaddr *>(&addr),
        reinterpret_cast<socklen_t *>(&addrlen));
  }

protected:
  port_t m_port{ cfg::port };
  addr_t m_addr{ cfg::addr };

};

} // namespace mqlqd

