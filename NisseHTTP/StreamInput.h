#ifndef THORSANVIL_NISSE_NISSEHTTP_STREAMINPUT_H
#define THORSANVIL_NISSE_NISSEHTTP_STREAMINPUT_H

#include "NisseHTTPConfig.h"
#include "Util.h"
#include <iostream>
#include <functional>
#include <vector>
#include <string_view>
#include <utility>

namespace ThorsAnvil::Nisse::HTTP
{

class StreamBufInput: public std::streambuf
{
    public:
        using Complete = std::function<void()>;
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;
        typedef traits::pos_type            pos_type;
        typedef traits::off_type            offtype;
    private:
        static std::streamsize constexpr chunkBufferSize = 1024;
        std::streamsize     remaining;
        std::streamsize     processed;
        std::streambuf*     buffer;
        bool                chunked;
        bool                firstChunk;
        Complete            complete;
        std::vector<char>   chunkBuffer;
    public:
        StreamBufInput(Complete&& complete = [](){});
        StreamBufInput(std::istream& stream, BodyEncoding encoding, Complete&& complete = [](){});
        StreamBufInput(StreamBufInput&& move)                   noexcept;
        StreamBufInput& operator=(StreamBufInput&& move)        noexcept;
        StreamBufInput(StreamBufInput const&)                   = delete;
        StreamBufInput& operator=(StreamBufInput const&)        = delete;

        std::string_view preloadStreamIntoBuffer();

        void swap(StreamBufInput& other) noexcept;
        friend void swap(StreamBufInput& lhs, StreamBufInput& rhs)   {lhs.swap(rhs);}

        // Create directly from socket.
    protected:
        // Read:
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override;
        virtual pos_type        seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out ) override;
        virtual pos_type        seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

    private:
        void checkBuffer();
        void getNextChunk();
        void preloadStreamIntoBufferNow();
        std::streamsize         currentPosition() const {return processed + (gptr() - eback());}

};

class StreamInput: public std::istream
{
    StreamBufInput    buffer;
    public:
        StreamInput()
            : std::istream(nullptr)
            , buffer()
        {}
        StreamInput(std::istream& stream, BodyEncoding encoding)
            : std::istream(nullptr)
            , buffer(stream, encoding)
        {
            rdbuf(&buffer);
        }

        void addBuffer(StreamBufInput&& newBuffer)
        {
            buffer = std::move(newBuffer);
            rdbuf(&buffer);
            clear();
        }
        std::string_view preloadStreamIntoBuffer()
        {
            return buffer.preloadStreamIntoBuffer();
        }
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "StreamInput.source"
#endif

#endif
