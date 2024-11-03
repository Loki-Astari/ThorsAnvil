#ifndef THORSANVIL_THORSSOCKET_SERVER_H
#define THORSANVIL_THORSSOCKET_SERVER_H

#include "ThorsSocketConfig.h"
#include "SocketUtil.h"
#include "Connection.h"
#include "Socket.h"
#include <variant>

namespace ThorsAnvil::ThorsSocket
{

using ServerInit = std::variant<ServerInfo, SServerInfo>;

class Server
{
    std::unique_ptr<ConnectionServer>   connection;
    YieldFunc                           yield;

    public:
        Server(ServerInit&& serverInit, Blocking blocking = Blocking::Yes);
        Server(ServerInfo&& socketInit, Blocking blocking = Blocking::Yes)  : Server{ServerInit{std::move(socketInit)}, blocking}{}
        Server(SServerInfo&& secureInit, Blocking blocking = Blocking::Yes) : Server{ServerInit{std::move(secureInit)}, blocking}{}
        Server(int port, Blocking blocking = Blocking::Yes)                 : Server{ServerInit{ServerInfo{port}}, blocking}{}
        Server(int port, SSLctx && ctx, Blocking blocking = Blocking::Yes)  : Server{ServerInit{SServerInfo{port, std::move(ctx)}}, blocking}{}
        ~Server();

        Server(Server&& move)               noexcept;
        Server& operator=(Server&& move)    noexcept;

        void swap(Server& other)            noexcept;

        Server(Server const&)               = delete;
        Server& operator=(Server const&)    = delete;

        bool isConnected()                  const;
        int  socketId(Mode rw)              const;      // Only useful for unit tests

        // Used by the Event Handler mechanism
        int  socketId()                     const   {return socketId(Mode::Read);}

        void close();
        void release();

        Socket accept(Blocking blocking = Blocking::Yes, DeferAccept deferAccept = DeferAccept::No);
        void setYield(YieldFunc&& yieldFunc)    {yield = std::move(yieldFunc);}
    private:
};
inline
void swap(Server& lhs, Server& rhs)
{
    lhs.swap(rhs);
}

}

#if THORS_SOCKET_HEADER_ONLY
#include "Server.source"
#endif

#endif
