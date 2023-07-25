#ifndef THORSANVIL_THORSSOCKET_CONNECTION_SOCKET_H
#define THORSANVIL_THORSSOCKET_CONNECTION_SOCKET_H

#include "ThorsSocketConfig.h"
#include "ConnectionUtil.h"
#include "ConnectionFileDescriptor.h"

#include <cstddef>

namespace ThorsAnvil::ThorsSocket::ConnectionType
{

using SocketAddrIn  = struct ::sockaddr_in;
using SocketAddr    = struct ::sockaddr;
using HostEnt       = struct ::hostent;

#ifdef __WINNT__
class Socket: public Connection
#else
class Socket: public ConnectionType::FileDescriptor
#endif
{
    SOCKET_TYPE fd;
    public:
        Socket(SocketInfo const& socketInfo, Blocking blocking);
        Socket(OpenSocketInfo const& socketInfo);
        virtual ~Socket();

        virtual bool isConnected()                          const   override;
        virtual int  socketId(Mode rw)                      const   override;
        virtual void close()                                        override;
        virtual void tryFlushBuffer()                               override;
        virtual void release()                                      override;

#if __WINNT__
        virtual IOData readFromStream(char* buffer, std::size_t size)       override;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  override;
#else
        virtual int getReadFD()                             const   override;
        virtual int getWriteFD()                            const   override;
#endif
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionSocket.source"
#endif

#endif
