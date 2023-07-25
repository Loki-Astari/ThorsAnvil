#ifndef THORSANVIL_DB_MONGO_THORS_MONGO_ACTION_H
#define THORSANVIL_DB_MONGO_THORS_MONGO_ACTION_H

#include "ThorsMongoConfig.h"
#include "MongoUtil.h"
#include "ThorSerialize/ThorsSerializerUtil.h"
#include "ThorSerialize/BsonPrinter.h"
#include "ThorSerialize/BsonThor.h"

#include <iostream>

namespace ThorsAnvil::DB::Mongo
{

using PrinterConfig = ThorsAnvil::Serialize::PrinterInterface::PrinterConfig;
using ParserConfig  = ThorsAnvil::Serialize::ParserInterface::ParserConfig;

// Interface.
// To be a message that can be sent to Mongo.
// The object should implement this interface.
// The default implementation is: MongoActionWriteInterfaceTrivialImpl
struct MongoActionWriteInterface
{
    MongoActionWriteInterface()                                             = default;
    MongoActionWriteInterface(MongoActionWriteInterface const&)             = default;
    MongoActionWriteInterface(MongoActionWriteInterface&&)                  = default;
    MongoActionWriteInterface& operator=(MongoActionWriteInterface const&)  = default;
    MongoActionWriteInterface& operator=(MongoActionWriteInterface&&)       = default;
    virtual ~MongoActionWriteInterface() {}

    virtual std::uint32_t   getSizeBson() const = 0;
    virtual void            writeBson(std::ostream& stream, PrinterConfig const& config) const = 0;
};
// Interface.
// To be a message that can be recieved from Mongo.
// The object should implement this interface.
// The default implementation is: MongoActionReadInterfaceTrivialImpl
struct MongoActionReadInterface
{
    MongoActionReadInterface()                                          = default;
    MongoActionReadInterface(MongoActionReadInterface const&)               = default;
    MongoActionReadInterface(MongoActionReadInterface&&)                    = default;
    MongoActionReadInterface& operator=(MongoActionReadInterface const&)    = default;
    MongoActionReadInterface& operator=(MongoActionReadInterface&&)         = default;
    virtual ~MongoActionReadInterface() {}

    virtual void            readBson(std::istream& stream, ParserConfig const& config) = 0;
};

template<typename T>
struct MongoActionWriteInterfaceTrivialImpl: public MongoActionWriteInterface
{
    virtual std::uint32_t    getSizeBson() const override
    {
        return ThorsAnvil::Serialize::bsonGetPrintSize(*dynamic_cast<T const*>(this));
    }
    virtual void            writeBson(std::ostream& stream, PrinterConfig const& config) const override
    {
        stream << ThorsAnvil::Serialize::bsonExporter(*dynamic_cast<T const*>(this), config);
    }
};

template<typename T>
struct MongoActionReadInterfaceTrivialImpl: public MongoActionReadInterface
{
    virtual void            readBson(std::istream& stream, ParserConfig const& config) override
    {
        stream >> ThorsAnvil::Serialize::bsonImporter(*dynamic_cast<T*>(this), config);
    }
};

class MessageHandler
{
    public:
        virtual ~MessageHandler() {}
        // Implement MongoWorker
        bool sendMessage(MongoActionWriteInterface const& section, MessageId& messageId, OP_MsgFlag flags, PrinterConfig const& config, bool handleChecksum = true);
        bool recvMessage(MongoActionReadInterface& section, MessageId messageId, ParserConfig const& config, bool handleChecksum = true);
        std::uint32_t calcSize(MongoActionWriteInterface const& action, bool hasChecksum) const;
        void setCompression(Compression compression);
        void setStreamBad();

        virtual std::iostream& getStream() = 0;
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "MessageHandler.source"
#endif

#endif
