#ifndef THORSANVIL_THORSSOCKET_CONNECTION_H
#define THORSANVIL_THORSSOCKET_CONNECTION_H

#include "ThorsSocketConfig.h"
#include "SocketUtil.h"

#include <cstddef>
#include <memory>
#include <string_view>

namespace ThorsAnvil::ThorsSocket
{

class ConnectionBase
{
    public:
        ConnectionBase()            {}
        virtual ~ConnectionBase()   {}
        ConnectionBase(ConnectionBase const&)                       = delete;
        ConnectionBase& operator=(ConnectionBase const&)            = delete;

        virtual bool isConnected()                          const   = 0;
        virtual int  socketId(Mode)                         const   = 0;
        virtual void close()                                        = 0;
        virtual void release()                                      = 0;
        virtual void externalyClosed()                              {}
};

class ConnectionClient: public ConnectionBase
{
    public:
        virtual void   tryFlushBuffer()                                     = 0;
        virtual IOData readFromStream(char* buffer, std::size_t size)       = 0;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  = 0;
        virtual std::string_view protocol() const                           = 0;
        virtual void   deferInit(YieldFunc&, YieldFunc&)               {}
};

class ConnectionServer: public ConnectionBase
{
    public:
        virtual std::unique_ptr<ConnectionClient> accept(YieldFunc& yield, Blocking blocking = Blocking::Yes, DeferAccept deferAccept = DeferAccept::No) = 0;
};

}

#endif
