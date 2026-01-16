#ifndef THORSANVIL_NISSE_NISSEHTTP_STREAMOUTPUT_H
#define THORSANVIL_NISSE_NISSEHTTP_STREAMOUTPUT_H

#include "NisseHTTPConfig.h"
#include "Util.h"
#include <iostream>
#include <functional>
#include <vector>
#include <utility>

namespace ThorsAnvil::Nisse::HTTP
{

class StreamBufOutput: public std::streambuf
{
    public:
        using Complete = std::function<void()>;
        typedef std::streambuf::traits_type traits;
        typedef traits::int_type            int_type;
        typedef traits::char_type           char_type;
    private:
        static std::streamsize constexpr chunkBufferSize = 1024;
        std::streamsize     remaining;
        std::streambuf*     buffer;
        bool                chunked;
        bool                firstChunk;
        Complete            complete;
        std::vector<char>   chunkBuffer;
    public:
        ~StreamBufOutput();
        StreamBufOutput(Complete&& complete = [](){});
        StreamBufOutput(std::ostream& stream, BodyEncoding bodyEncoding, Complete&& complete = [](){});
        StreamBufOutput(StreamBufOutput&& move)                   noexcept;
        StreamBufOutput& operator=(StreamBufOutput&& move)        noexcept;
        StreamBufOutput(StreamBufOutput const&)                   = delete;
        StreamBufOutput& operator=(StreamBufOutput const&)        = delete;

        void swap(StreamBufOutput& other) noexcept;
        friend void swap(StreamBufOutput& lhs, StreamBufOutput& rhs)   {lhs.swap(rhs);}

        void done();

        // Create directly from socket.
    protected:
        // Control:
        virtual int             sync() override;

        // Write:
        virtual std::streamsize xsputn(char_type const*,std::streamsize) override;
        virtual int_type        overflow(int_type = traits::eof()) override;

    private:
        void checkBuffer();
        void outputChunkSize(std::streamsize size);
        char toHex(int digit);
        void sendAllData(char const* s, std::streamsize size);

        std::streamsize xsputnChunked(char_type const*,std::streamsize);
        std::streamsize xsputnLength(char_type const*,std::streamsize);
        int_type        overflowChunked(int_type = traits::eof());
        int_type        overflowLength(int_type = traits::eof());
        void            dumpBuffer();
};

class StreamOutput: public std::ostream
{
    StreamBufOutput    buffer;
    public:
        StreamOutput()
            : std::ostream(nullptr)
            , buffer()
        {}
        StreamOutput(std::ostream& stream, BodyEncoding encoding)
            : std::ostream(nullptr)
            , buffer(stream, encoding)
        {
            rdbuf(&buffer);
        }

        void addBuffer(StreamBufOutput&& newBuffer)
        {
            buffer = std::move(newBuffer);
            rdbuf(&buffer);
            clear();
        }
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "StreamOutput.source"
#endif

#endif
