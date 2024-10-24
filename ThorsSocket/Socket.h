#ifndef THORSANVIL_THORSSOCKET_SOCKET_H
#define THORSANVIL_THORSSOCKET_SOCKET_H

#include "ThorsSocketConfig.h"
#include "SocketUtil.h"

#include <memory>
#include <functional>
#include <cstddef>
#include <variant>

namespace ThorsAnvil::ThorsSocket
{

class ConnectionClient;
enum class TestMarker {True};

using YieldFunc     = std::function<bool()>;
using SocketInit    = std::variant<FileInfo, PipeInfo, SocketInfo, SSocketInfo>;

class Server;
class Socket
{
    std::unique_ptr<ConnectionClient>   connection;
    YieldFunc                           readYield;
    YieldFunc                           writeYield;

    friend class Server;
    Socket(std::unique_ptr<ConnectionClient>&& connection);
    public:
        Socket();
        Socket(SocketInit const& initInfo, Blocking blocking = Blocking::Yes);
        ~Socket();

        // Good for testing only.
        template<typename T>
        Socket(TestMarker, T const& socket)
            : connection(std::make_unique<typename T::Connection>(socket))
            , readYield([](){return false;})
            , writeYield([](){return false;})
        {}

        Socket(Socket&& move)               noexcept;
        Socket& operator=(Socket&& move)    noexcept;

        void swap(Socket& other)            noexcept;

        Socket(Socket const&)               = delete;
        Socket& operator=(Socket const&)    = delete;

        bool isConnected()                  const;
        int  socketId(Mode rw)              const;      // Only useful for unit tests

        // Used by the Event Handler mechanism
        int  socketId()                     const   {return socketId(Mode::Read);}

        IOData getMessageData(void* buffer, std::size_t size);
        IOData tryGetMessageData(void* buffer, std::size_t size);
        IOData putMessageData(void const* buffer, std::size_t size);
        IOData tryPutMessageData(void const* buffer, std::size_t size);

        void tryFlushBuffer();

        void close();
        void release();
        void externalyClosed();

        void setReadYield(YieldFunc&& yield)    {readYield = std::move(yield);}
        void setWriteYield(YieldFunc&& yield)   {writeYield = std::move(yield);}
        std::string_view protocol();
    private:
        IOData getMessageDataFromStream(void* b, std::size_t size, bool waitWhenBlocking);
        IOData putMessageDataToStream(void const* b, std::size_t size, bool waitWhenBlocking);
        void waitForInput();
        void waitForOutput();
        void waitForFileDescriptor(int fd, short flag);
};
inline
void swap(Socket& lhs, Socket& rhs)
{
    lhs.swap(rhs);
}

}

#if THORS_SOCKET_HEADER_ONLY
#include "Socket.source"
#endif

#endif
