#ifndef THORSANVIL_THORSSOCKET_SOCKET_STREAM_TPP
#define THORSANVIL_THORSSOCKET_SOCKET_STREAM_TPP

namespace ThorsAnvil::ThorsSocket
{

template<typename Buffer>
SocketStream<Buffer>::SocketStream(PipeInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
SocketStream<Buffer>::SocketStream(FileInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
SocketStream<Buffer>::SocketStream(SocketInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
SocketStream<Buffer>::SocketStream(SSocketInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
SocketStream<Buffer>::SocketStream(SocketStream&& move) noexcept
    : std::iostream(nullptr)
    , buffer(std::move(move.buffer))
{
    rdbuf(&buffer);
}

}

#endif
