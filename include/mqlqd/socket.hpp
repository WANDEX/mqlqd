#pragma once
/**
 * !!! Project Does Not Use This Header !!!
 * Abstraction layer on top of the Linux socket API.
 * (wrapper around the user API C language functions).
 * Socket Base class common to the both targets:
 * client (app) & server (daemon).
 * 'https://isocpp.org/wiki/faq/mixing-c-and-cpp'
 *
 * !!!!!!!!!!!!!!!!!!!!
 * !!! EXPERIMENTAL !!!
 * !!!  UNFINISHED  !!!
 * !!!   NOT USED   !!!
 * !!!!!!!!!!!!!!!!!!!!
 *
 * Now it seems like a bad idea...
 * I left this header here just for my notes!
 * !!! Project Does Not Use This Header !!!
 */

namespace mqlqd {
// TODO maybe also nest this into the anonymous namespace for the internal linkage?
namespace /* (anonymous) */ {

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

/*
 * #include <sys/types.h>
 * #include <sys/socket.h>
 * #include <netdb.h>
 * declared in those headers:
 */

// FIXME not match the original function signature.
int getaddrinfo(const char *node,
                const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

// void freeaddrinfo(struct addrinfo *res);

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

/*
 * @brief look into $ man 2 connect .
 * #include <sys/socket.h>
 *
 * original function signature looks like this!
 * int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 * => FIXME: or ok?
 */
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*
 * @brief man manual send(2).
 * #include <sys/socket.h>
 *
 * original function signature looks like this!
 * ssize_t send(int sockfd, const void buf[.len], size_t len, int flags);
 * => FIXME: or ok?
 */
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

/*
 * @brief man manual recv(2).
 * #include <sys/socket.h>
 *
 * original function signature looks like this!
 * ssize_t recv(int sockfd, void buf[.len], size_t len, int flags);
 * => FIXME: or ok?
 */
ssize_t recv(int sockfd, void *buf, size_t len, int flags);

/*
 * @brief man manual close(2).
 * #include <unistd.h>
 */
int close(int fd);

} // extern "C"
} // (anonymous) [internal_linkage]


/**
 * @brief Socket RAW Wrapper
 */
namespace sucks {

[[nodiscard]] int inline
getaddrinfo(const char *node,
            const char *service,
            const struct addrinfo *hints,
            struct addrinfo **res)
{
  return mqlqd::getaddrinfo
    (
      reinterpret_cast<char *>(&node),
      reinterpret_cast<char *>(&service),
      reinterpret_cast<struct addrinfo *>(&hints),
      reinterpret_cast<struct addrinfo **>(&res)
    ); // XXX: ok or wrong?
}

[[nodiscard]] int inline
socket(int domain, int type, int protocol)
{
  return mqlqd::socket(domain, type, protocol);
}

[[nodiscard]] int inline
bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  return mqlqd::bind(sockfd, reinterpret_cast<struct sockaddr *>(&addr), addrlen);
}

[[nodiscard]] int inline
listen(int sockfd, int backlog)
{
  return mqlqd::listen(sockfd, backlog);
}

// [[nodiscard]] int
// accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);

// FIXME: the method signature differ from the original function!
[[nodiscard]] int inline
accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  // XXX do we need those casts or not? ...
  return mqlqd::accept(sockfd,
      reinterpret_cast<struct sockaddr *>(&addr),
      reinterpret_cast<socklen_t *>(&addrlen));
}

[[nodiscard]] int inline
connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  // XXX do we need those casts or not? ...
  // FIXME: certainly not ok! (sizeof addr & multiplication is needed???)
  return mqlqd::connect(sockfd,
      reinterpret_cast<struct sockaddr *>(&addr),
      addrlen); // XXX: ok?
}

// XXX: !!! originally returns ssize_t
//      Do i need to static_cast it?
// [[nodiscard]] ssize_t
[[nodiscard]] intmax_t inline
send(int sockfd, const void *buf, size_t len, int flags)
{
  // FIXME do i wrote it correct?
  return mqlqd::send(sockfd, &buf, len, flags);
}


// XXX: !!! originally returns ssize_t
//      Do i need to static_cast it?
// [[nodiscard]] ssize_t
[[nodiscard]] intmax_t inline
recv(int sockfd, const void *buf, size_t len, int flags)
{
  // FIXME do i wrote it correct?
  return mqlqd::recv(sockfd, &buf, len, flags);
}

[[nodiscard]] int inline
close(int fd) noexcept
{
  return mqlqd::close(fd);
}

} // namespace sucks

} // namespace mqlqd

