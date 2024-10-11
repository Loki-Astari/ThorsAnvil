#ifndef THORSANVIL_THORSSOCKET_SERVER_H
#define THORSANVIL_THORSSOCKET_SERVER_H

#include "ThorsSocketConfig.h"
#include "SocketUtil.h"
#include "Connection.h"
#include "Socket.h"

namespace ThorsAnvil::ThorsSocket
{

class Server
{
    std::unique_ptr<ConnectionServer>   connection;
    YieldFunc                           readYield;
    YieldFunc                           writeYield;

    public:
        Server(ServerInfo const& socket, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        Server(SServerInfo const& socket, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
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

        Socket accept(Blocking blocking = Blocking::Yes, AcceptFunc&& acceptOK = [](){}, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
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
