#ifndef THORSANVIL_THORSSOCKET_SOCKET_STREAM_H
#define THORSANVIL_THORSSOCKET_SOCKET_STREAM_H

#include "ThorsSocketConfig.h"
#include "SocketStreamBuffer.h"

#include <iostream>

namespace ThorsAnvil::ThorsSocket
{


template<typename Buffer = SocketStreamBuffer>
class BaseSocketStream: public std::iostream
{
    Buffer  buffer;

    public:
        // Default: (no connection attached) constructor.
        BaseSocketStream();

        // Create from an existing socket.
        BaseSocketStream(Socket&& socket);
        BaseSocketStream(BaseSocketStream&& move) noexcept;
        BaseSocketStream& operator=(BaseSocketStream&& move) noexcept;

        // Manual Creation.
        BaseSocketStream(PipeInfo const& info);
        BaseSocketStream(FileInfo const& info);
        BaseSocketStream(SocketInfo const& info);
        BaseSocketStream(SSocketInfo const& info);

        // Destructor
        ~BaseSocketStream()                                             = default;

        // No copying allowed
        BaseSocketStream(BaseSocketStream const&)                       = delete;
        BaseSocketStream& operator=(BaseSocketStream const&)            = delete;

        // Usefult for testing
        Socket&         getSocket()         {return buffer.getSocket();}
        Socket const&   getSocket() const   {return buffer.getSocket();}
        operator bool()             const   {return buffer.getSocket().isConnected();}
};

using SocketStream = BaseSocketStream<SocketStreamBuffer>;

}

#include "SocketStream.tpp"

#endif
