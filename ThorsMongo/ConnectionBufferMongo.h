#ifndef THORSANVIL_DB_CONNECTION_BUFFER_MONGO_H
#define THORSANVIL_DB_CONNECTION_BUFFER_MONGO_H

#include "ThorsMongoConfig.h"
#include "MongoUtil.h"
#include "ThorsSocket/SocketStreamBuffer.h"
#include "ThorsCrypto/crc.h"


namespace ThorsAnvil::DB::Mongo
{

/*
 * The ConnectionBufferMongo understands that communication is done in message blocks.
 * The format of the message includes the size of the message in the first 4 bytes.
 * Also that the minimum message size is 26 bytes.
 *      MessageHeader:      16 bytes.
 *      MessageInfo:
 *          Flags           4 bytes.
 *          Kind            1 bytes
 *          BSON doc                    5 bytes min
 *              size        4 bytes
 *              terminator  1 byte
 *                      -----
 *                         26 bytes.
 *
 * Reading:
 *      So if we are not in a message this will force underflow to get the message block.
 *      So underflow()
 *          We read the size of the message.
 *          This will block.
 *          Once we have the size we will try and read the full the message.
 *          This full read will not block if we can't read the full message.
 *          But will block to make sure the underlying read operation will complete.
 *              i.e. You tried to read 8 bytes of data and trying to read the message
 *                   only returned 6 bytes it will block until you have all 8.
 *          Note:
 *              Because we don't block on a full read we may read a partial message.
 *              This means that underflow() may be called again when we need more data.
 *              If this happens we will always try and read (non blocking) the rest of
 *              the message, but will block to make sure the underlying read can complete.
 *
 *          We use the "underflowNeeds" to communicate the minimum number of bytes needed.
 *
 *      The xsgetn() is overridden to allow for efficiency.
 *          This will call underflow() if more data is needed.
 *          But this will always copy data into the buffer.
 *
 *          The reason for this is we need all the data in the buffer to perform
 *          a checksum check when the message is complete.
 *
 * Writing:
 *      When writing the full message is queued in the buffer and only flushed on sync().
 *      Trying to sync() before the message is complete is an error.
 *
 *      This is need to allow us to compute the checksum value.
 *      Also this will (next version) allow us to compress the output message.
 */
class ConnectionBufferMongo: public ThorsSocket::SocketStreamBuffer
{
    using traits      = std::streambuf::traits_type;
    using int_type    = traits::int_type;
    using char_type   = traits::char_type;

    using Checksum    = ThorsAnvil::Crypto::CRC32C_Checksum;

    std::uint32_t   inMessageSize           = 0;
    std::uint32_t   outMessageSize          = 0;
    std::uint32_t   underflowNeeds          = 1;
    std::uint32_t   outMessagePlaced        = 0;   // Used to make sure we get the bytes indicating message size.
    Compression     useCompressionOnWrite   = Compression::None;

    public:
        using SocketStreamBuffer::SocketStreamBuffer;
        ConnectionBufferMongo(ConnectionBufferMongo&& move) noexcept;

        void setCompressionOnWrite(Compression comp) {useCompressionOnWrite = comp;}

    protected:
        virtual int_type        underflow() override;
        virtual std::streamsize xsgetn(char_type* dest, std::streamsize count) override;

        virtual int_type        overflow(int_type ch = traits::eof()) override;
        virtual std::streamsize xsputn(char_type const* source, std::streamsize count) override;

        virtual int sync() override;
    private:
        std::uint32_t   messageRetrievedSize()  const {return egptr() - eback();}
        std::uint32_t   messageReadSize()       const {return gptr() - eback();}
        std::uint32_t   missingData()           const {return inMessageSize - messageRetrievedSize();}
        std::uint32_t   availableData()         const {return egptr() - gptr();}
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ConnectionBufferMongo.source"
#endif

#endif
