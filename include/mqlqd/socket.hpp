#pragma once
// SocketBase common for the both targets: client & daemon

#include "aliases.hpp"
#include "config.hpp"


namespace mqlqd {

class SocketBase
{
public:
  SocketBase();
  SocketBase(SocketBase &&) = delete;
  SocketBase(const SocketBase &) = delete;
  SocketBase &operator=(SocketBase &&) = delete;
  SocketBase &operator=(const SocketBase &) = delete;
  virtual ~SocketBase() noexcept;

protected:
  port_t m_port{ cfg::port };
  addr_t m_addr{ cfg::addr };

};

} // namespace mqlqd

