#ifndef THORSANVIL_THORSSOCKET_SOCKET_STREAM_TPP
#define THORSANVIL_THORSSOCKET_SOCKET_STREAM_TPP

namespace ThorsAnvil::ThorsSocket
{

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream()
    : std::iostream(nullptr)
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(Socket&& socket)
    : std::iostream(nullptr)
    , buffer(std::move(socket))
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(BaseSocketStream&& move) noexcept
    : std::iostream(nullptr)
    , buffer(std::move(move.buffer))
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>& BaseSocketStream<Buffer>::operator=(BaseSocketStream&& move) noexcept
{
    std::iostream::operator=(std::move(move));
    buffer = std::move(move.buffer);
    rdbuf(&buffer);
    return *this;
}


template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(PipeInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(FileInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(SocketInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

template<typename Buffer>
BaseSocketStream<Buffer>::BaseSocketStream(SSocketInfo const& info)
    : std::iostream(nullptr)
    , buffer(info)
{
    rdbuf(&buffer);
}

}

#endif
