#pragma once
/*
 * For reference visit man manual pages:
 * socket(2)
 * sockaddr(3type)
 * netdb.h(0P)
 *
 * etc.
 * $ man 3type sock
 * TAB compl. here ^
 */

// for the project-wide type aliases
#include "aliases.hpp"


namespace /* (anonymous) */ {
extern "C" {

// #include <sys/socket.h>

} // extern "C"
} // (anonymous) [internal_linkage]


namespace mqlqd {


struct sockaddr; // ok?

// struct sockaddr {
//   sa_family_t     sa_family;      /* Address family */
//   char            sa_data[];      /* Socket address */
// }; // FIXME: what to do   ^ with this?
// XXX Flexible array members are a C99 feature [-Wc99-extensions]

struct sockaddr_storage {
  sa_family_t     ss_family;      /* Address family */
};

/*
 * Internet domain sockets
 * #include <netinet/in.h>
 */

struct in_addr {
  in_addr_t s_addr;
};

struct sockaddr_in {
  sa_family_t     sin_family;     /* AF_INET */
  in_port_t       sin_port;       /* Port number */
  struct in_addr  sin_addr;       /* IPv4 address */
};

/*
 * UNIX domain sockets
 * #include <sys/un.h>
 */

// struct sockaddr_un {
//   sa_family_t     sun_family;     /* Address family */
//   char            sun_path[];     /* Socket pathname */
// };

// XXX should i remove default value initialize or ok?
struct addrinfo {
  int              ai_flags     { };
  int              ai_family    { };
  int              ai_socktype  { };
  int              ai_protocol  { };
  socklen_t        ai_addrlen   { };
  struct sockaddr *ai_addr      { };
  char            *ai_canonname { };
  struct addrinfo *ai_next      { };
};

} // namespace mqlqd

