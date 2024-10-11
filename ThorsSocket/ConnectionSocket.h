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

class SocketStandard
{
    SOCKET_TYPE fd;
    public:
        SocketStandard(ServerInfo const& socketInfo, Blocking blocking);
        SocketStandard(SocketInfo const& socketInfo, Blocking blocking);
        SocketStandard(OpenSocketInfo const& socketInfo, Blocking blocking);
        virtual ~SocketStandard();

        bool isConnected()          const;
        int  socketId(Mode rw)      const;
        void close();
        void release();

        int getFD()                 const;
    private:
        void createSocket();
        void setUpBlocking(Blocking blocking);
        void setUpServerSocket(ServerInfo const& socketInfo);
        void setUpClientSocket(SocketInfo const& socketInfo);
};

class SocketServer;
#ifdef __WINNT__
class SocketClient: public ConnectionClient
#else
class SocketClient: public ConnectionType::FileDescriptor
#endif
{
    SocketStandard  socketInfo;
    public:
        // Normal Client.
        SocketClient(SocketInfo const& socketInfo, Blocking blocking);
        // Server Side accept.
        SocketClient(SocketServer&, OpenSocketInfo const& socketInfo, Blocking blocking);
        virtual ~SocketClient();

        virtual bool isConnected()                          const   override;
        virtual int  socketId(Mode rw)                      const   override;
        virtual void close()                                        override;
        virtual void release()                                      override;

        virtual void tryFlushBuffer()                               override;
#ifdef __WINNT__
        virtual IOData readFromStream(char* buffer, std::size_t size)       override;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  override;
#else
        virtual int getReadFD()                             const   override;
        virtual int getWriteFD()                            const   override;
#endif
};

class SocketServer: public ConnectionServer
{
    SocketStandard  socketInfo;
    public:
        SocketServer(ServerInfo const& socketInfo, Blocking blocking);
        virtual ~SocketServer();

        virtual bool isConnected()                          const   override;
        virtual int  socketId(Mode rw)                      const   override;
        virtual void close()                                        override;
        virtual void release()                                      override;

        virtual std::unique_ptr<ConnectionClient> accept(Blocking blocking, AcceptFunc&& accept = [](){})          override;
    protected:
        int acceptSocket(AcceptFunc&& accept);
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionSocket.source"
#endif

#endif
