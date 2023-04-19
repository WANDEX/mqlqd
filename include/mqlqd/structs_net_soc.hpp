#pragma once
/*
 * This header file used only for the Notes!
 *
 * For reference visit man manual pages:
 * getaddrinfo(3)
 * socket(2)
 * bind(2)
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

// For the operating system specific stuff.
// here it specifically for the - struct sockaddr!
#include <sys/socket.h>

} // extern "C"
} // (anonymous) [internal_linkage]


namespace mqlqd {


/**
 * Reference and description/explanation taken from and can be found in the manual page:
 * man 2 bind
 *
 * The actual structure passed for the addr argument will depend on the address family.
 * The sockaddr structure is defined as something like:
 *
 * struct sockaddr {
 *   sa_family_t sa_family;
 *   char        sa_data[14];
 * }
 *
 * The only purpose of this structure is
 * to cast the structure pointer passed in addr in order to avoid compiler warnings.
 *
 * Look up examples in the bind(2) manual page.
 */

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

