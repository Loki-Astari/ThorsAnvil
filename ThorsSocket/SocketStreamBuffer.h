#ifndef THORSANVIL_THORSSOCKET_SOCKET_STREAM_BUFFER_H
#define THORSANVIL_THORSSOCKET_SOCKET_STREAM_BUFFER_H

#include "ThorsSocketConfig.h"
#include "Socket.h"

#include <iostream>
#include <vector>
#include <cstddef>

namespace ThorsAnvil::ThorsSocket
{

class SocketStreamBuffer: public std::streambuf
{
    typedef std::streambuf::traits_type traits;
    typedef traits::int_type            int_type;
    typedef traits::char_type           char_type;

    Socket                  socket;
    std::vector<char>       inputBuffer;
    std::vector<char>       outputBuffer;
    std::size_t             inCount;
    std::size_t             outCount;
    public:
        // Create directly from socket.
        SocketStreamBuffer();
        SocketStreamBuffer(Socket&& socket);
        SocketStreamBuffer(SocketStreamBuffer&& move) noexcept;
        SocketStreamBuffer& operator=(SocketStreamBuffer&&) noexcept;

        // Create the socket internally.
        SocketStreamBuffer(PipeInfo const& info);
        SocketStreamBuffer(FileInfo const& info);
        SocketStreamBuffer(SocketInfo const& info);
        SocketStreamBuffer(SSocketInfo const& info);
        ~SocketStreamBuffer();

        // Don't allow copy
        SocketStreamBuffer(SocketStreamBuffer const&)                       = delete;
        SocketStreamBuffer& operator=(SocketStreamBuffer const&)            = delete;

        // Useful for testing
        Socket&         getSocket()         {return socket;}
        Socket const&   getSocket() const   {return socket;}

        // This is useful if you want to force the buffer to read a certain amount without copying out of the buffer.
        // This is good on compressed strreams. We can make sure that the whole compressed block has been read.
        // Then decompress it inside the buffer pretending nothing has happened.
        // See MongoBuffer for an example
        void reserveInputSize(std::size_t size);
        void reserveOutputSize(std::size_t size);
    protected:
        void    swap(SocketStreamBuffer& rhs) noexcept;
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;

        virtual int_type        overflow(int_type ch = traits::eof()) override;
        virtual std::streamsize xsputn(char_type const* s,std::streamsize count) override;

        virtual int             sync() override;
        virtual std::streampos  seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

        void incrementInCount(std::size_t size)     {inCount    += size;}
        void incrementOutCount(std::size_t size)    {outCount   += size;}
    protected:
        // Used by derived class to write.
        std::streamsize writeToStream(char const* data, std::size_t size);
        std::streamsize readFromStream(char* data, std::size_t size);
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "SocketStreamBuffer.source"
#endif

#endif
