#ifndef THORSANVIL_NISSE_CONTEXT_H
#define THORSANVIL_NISSE_CONTEXT_H

#include "NisseServerConfig.h"
#include "NisseUtil.h"
#include "EventHandlerLibEvent.h"
#include <ThorsSocket/Socket.h>
#include <ThorsSocket/SocketStream.h>

namespace TASock = ThorsAnvil::ThorsSocket;

namespace ThorsAnvil::Nisse::Server
{

class NisseServer;

class Context
{
    NisseServer&    server;
    Yield&          yield;
    int             owner;
    public:
        Context(NisseServer& server, Yield& yield, int owner);
        void registerOwnedSocketStream(TASock::SocketStream& stream, EventType initialWait);
        void unregisterOwnedSocketStream(TASock::SocketStream& stream);
        void registerOwnedSocket(TASock::Socket& socket, EventType initialWait);
        void unregisterOwnedSocket(TASock::Socket& socket);
        bool isFeatureEnabled(Feature feature) const;
        static void registerSharedSocket(NisseServer& server, TASock::Socket& socket);
        static void unregisterSharedSocket(NisseServer& server, TASock::Socket& socket);

        Yield&      getYield()  {return yield;}
    private:
        void registerYield(int fd, EventType initialWait, TASock::Socket& socket, TASock::YieldFunc&& readYield, TASock::YieldFunc&& writeYield);
        void unregisterYield(int fd);
};

// The RAII classes to correctly utilize the Context object.
class AsyncStream
{
    TASock::SocketStream&   stream;
    Context&                context;
    public:
        AsyncStream(TASock::SocketStream& stream, Context& context, EventType initialWait);
        ~AsyncStream();
};

class AsyncSocket
{
    TASock::Socket&         socket;
    Context&                context;
    public:
        AsyncSocket(TASock::Socket& socket, Context& context, EventType initialWait);
        ~AsyncSocket();
};

class AsyncSharedSocket
{
    TASock::Socket&         socket;
    NisseServer&            server;
    public:
        AsyncSharedSocket(TASock::Socket& socket, NisseServer& server);
        ~AsyncSharedSocket();
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#endif

#endif
