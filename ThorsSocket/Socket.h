#ifndef THORSANVIL_THORSSOCKET_SOCKET_H
#define THORSANVIL_THORSSOCKET_SOCKET_H

#include "ThorsSocketConfig.h"
#include "SocketUtil.h"

#include <memory>
#include <functional>
#include <cstddef>

namespace ThorsAnvil::ThorsSocket
{

class Connection;
enum class TestMarker {True};

using YieldFunc     = std::function<bool()>;

class Socket
{
    std::unique_ptr<Connection>     connection;
    YieldFunc                       readYield;
    YieldFunc                       writeYield;

    public:
        Socket();
        Socket(FileInfo const& file, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        Socket(PipeInfo const& pipe, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        Socket(SocketInfo const& socket, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        Socket(SSocketInfo const& socket, Blocking blocking = Blocking::Yes, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        // --
        Socket(OpenSocketInfo const& socket, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        Socket(OpenSSocketInfo const& socket, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;});
        ~Socket();

        // Good for testing only.
        template<typename T>
        Socket(TestMarker, T const& socket, YieldFunc&& readYield = [](){return false;}, YieldFunc&& writeYield = [](){return false;})
            : connection(std::make_unique<typename T::Connection>(socket))
            , readYield(std::move(readYield))
            , writeYield(std::move(writeYield))
        {}

        Socket(Socket&& move)               noexcept;
        Socket& operator=(Socket&& move)    noexcept;

        void swap(Socket& other)            noexcept;

        Socket(Socket const&)               = delete;
        Socket& operator=(Socket const&)    = delete;

        bool isConnected()                  const;
        int  socketId(Mode rw)              const;      // Only useful for unit tests

        IOData getMessageData(void* buffer, std::size_t size);
        IOData tryGetMessageData(void* buffer, std::size_t size);
        IOData putMessageData(void const* buffer, std::size_t size);
        IOData tryPutMessageData(void const* buffer, std::size_t size);

        void tryFlushBuffer();

        void close();
        void release();
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
