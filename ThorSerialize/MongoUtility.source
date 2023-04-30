#include "MongoUtility.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <tuple>
#include <arpa/inet.h>

using namespace ThorsAnvil::Serialize;
using namespace ThorsAnvil::Serialize::MongoUtility;

int ObjectID::classCounter = 32769;

ObjectID::ObjectID(std::int32_t timestamp, std::int64_t random, std::int32_t counter)
    : timestamp(timestamp)
    , random(random)
    , counter(counter)
{}

namespace ThorsAnvil::Serialize::MongoUtility
{
    BsonPrinter& operator<<(BsonPrinter& printer, ObjectID const& data)
    {
        printer.writeBE<4>(data.timestamp);
        printer.writeBE<5>(data.random);
        printer.writeBE<3>(data.counter);
        return printer;
    }
    JsonPrinter& operator<<(JsonPrinter& printer, ObjectID const& data)
    {
        printer.stream() << ThorsAnvil::Utility::StreamFormatterNoChange{}
                         << "\""
                         << std::hex << std::setfill('0')
                         << std::setw( 8) << data.timestamp << "-"
                         << std::setw(10) << data.random    << "-"
                         << std::setw( 6) << data.counter
                         << "\"";
        return printer;
    }
    BsonParser& operator>>(BsonParser& parser, ObjectID& data)
    {
        data.timestamp = parser.readBE<4, std::int32_t>();
        data.random    = parser.readBE<5, std::int64_t>();
        data.counter   = parser.readBE<3, std::int32_t>();
        return parser;
    }
    JsonParser& operator>>(JsonParser& parser, ObjectID& data)
    {
        char x1, x2, x3, x4;
        parser.stream() >> ThorsAnvil::Utility::StreamFormatterNoChange{} >> std::hex >> x1 >> data.timestamp >> x2 >> data.random >> x3 >> data.counter >> x4;
        return parser;
    }
}

UTCDateTime::UTCDateTime(std::int64_t datetime)
    : datetime(datetime)
{}
namespace ThorsAnvil::Serialize::MongoUtility
{
    BsonPrinter& operator<<(BsonPrinter& printer, UTCDateTime const& data)
    {
        printer.writeLE<8, std::int64_t>(data.datetime);
        return printer;
    }
    JsonPrinter& operator<<(JsonPrinter& printer, UTCDateTime const& data)
    {
        printer.stream() << ThorsAnvil::Utility::StreamFormatterNoChange{} << std::hex << std::setw(16) << std::setfill('0') << data.datetime;
        return printer;
    }
    BsonParser& operator>>(BsonParser& parser, UTCDateTime& data)
    {
        data.datetime = parser.readLE<8, std::int64_t>();
        return parser;
    }
    JsonParser& operator>>(JsonParser& parser, UTCDateTime& data)
    {
        parser.stream() >> ThorsAnvil::Utility::StreamFormatterNoChange{} >> std::hex >> data.datetime;
        return parser;
    }
}

BsonTimeStamp::BsonTimeStamp(std::time_t timestamp, int inc)
    : increment(inc)
    , timestamp(timestamp)
{}

UTCDateTime BsonTimeStamp::asDateTime()
{
    return UTCDateTime(timestamp * 1000);
}

namespace ThorsAnvil::Serialize::MongoUtility
{
    BsonPrinter& operator<<(BsonPrinter& printer, BsonTimeStamp const& data)
    {
        printer.writeBE<4, std::int32_t>(data.increment);
        printer.writeBE<4, std::int32_t>(data.timestamp);
        return printer;
    }
    JsonPrinter& operator<<(JsonPrinter& printer, BsonTimeStamp const& data)
    {
        printer.stream() << ThorsAnvil::Utility::StreamFormatterNoChange{}
                         << "\""
                         << std::hex << std::setw(8) << std::setfill('0')
                         << data.timestamp << "-"
                         << data.increment
                         << "\"";
        return printer;
    }
    BsonParser& operator>>(BsonParser& parser, BsonTimeStamp& data)
    {
        data.increment = parser.readBE<4, std::int32_t>();
        data.timestamp = parser.readBE<4, std::int32_t>();
        return parser;
    }
    JsonParser& operator>>(JsonParser& parser, BsonTimeStamp& data)
    {
        char x1, x2, x3;
        parser.stream() >> ThorsAnvil::Utility::StreamFormatterNoChange{} >> std::hex >> x1 >> data.timestamp >> x2 >> data.increment >> x3;
        return parser;
    }
}
