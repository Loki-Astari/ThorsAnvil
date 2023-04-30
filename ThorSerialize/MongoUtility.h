#ifndef THORS_ANVIL_SERIALIZE_MONGO_UTILITY_H
#define THORS_ANVIL_SERIALIZE_MONGO_UTILITY_H

#include "BsonParser.h"
#include "BsonPrinter.h"
#include "Exporter.h"
#include "Importer.h"
#include "SerUtil.h"
#include "CustomSerialization.h"
#include "ThorsIOUtil/Utility.h"

#include "BsonThor.h"
#include "JsonThor.h"
#include <chrono>

namespace ThorsAnvil
{
    namespace Serialize
    {
        namespace MongoUtility
        {

/*
 * BSON Elements
 *  '\x01'      POD: float/double
 *  '\x02'      std::string
 *  '\x03'      All the following work:
 *                  Structure
 *                  std::map<std::string, V>/std::unordered_map<std::string, V>
 *                  std::multimap<std::string, V>/std::unordered_multimap<std::string, V>
 *  '\x04'      All the following work:
 *                  std::vector<V>
 *                  std::array<V>
 *                  initializer_list<V>
 *                  std::list<V>
 *                  std::dequeu<V>
 *                  std::set<V>/std::multiset<V>
 *                  std::unordered_set<V>/std::unordered_multiset<V>
 *                  std::map<K, V>/std::unordered_map<K, V>             => Key Value pairs
 *                  std::multimap<K, V>/std::unordered_multimap<K, V>   => Key Value pairs
 *  '\x05'      Use DataInterface and BinarySerializer for Serialization
 *  '\x07'      Use ObjectID        or Use NormalSerializationInterface and ObjectIDSerializer for Serialization
 *  '\x08'      POD bool
 *  '\x09'      Use UTCDateTime     or Use NormalSerializationInterface and DataTimeSerializer
 *  '\x0A'      nullptr
 *  '\x0B'      use RegExInterface and RegExSerializer for Serialization
 *  '\x0D'      use DataInterface and JavascriptSerializer for Serialization
 *  '\x10'      Integer types that are shorter or equal to 32 bits: short/int/long
 *  '\x11'      use NormalSerializationInterface and TimeStampSerializer for Serialization
 *  '\x12'      Integer types that are longer or equal to 64 bits:  long long
 *  '\x13'      POD long double (if long double is IEEE compliant)
 *
 * Not Implemented yet.
 *  '\xFF' e_name Min key
 *  '\x7F' e_name Max key
 *
 * Deprecated in standard and not used.
 *  '\x06'
 *  '\x0C'
 *  '\x0E'
 *  '\x0F'
 *
 *
 * For the terms:
 *  Use <X>Interface and <Y> for Serialization
 *  This means make the type you want to serialize implement the interface X (described below)
 *  Then add the following declaration:
 *      class MyType: public <X>Interface
 *      {
 *          Note: You don't actually want to inherit from these interfaces.
 *                This is a Duck type interface. You wan to implement the functions
 *                specified by the interface only.
 *      };
 *
 *      ThorsAnvil_MakeTraitCustomSerialize(MyType, <Y>);
 *
 * The special one here is the 'NormalSerialization* interface.
 * Rather than implement a specific interface you simply implement symetric serialization functions.
 * i.e. overload the `operator<<` and `operator>>`
 */

#if 0
class NormalSerializationInterface
{
    friend std::ostream& operator<<(std::ostream& stream, ObjectID const& data);
    friend std::istream& operator<<(std::istream& stream, ObjectID& data);
};
class DataInterface
{
    public:
        std::size_t getSize() const             = 0;
        void        resize(std::size_t size)    = 0;
        char*       getBuffer()                 = 0;
};
class RegExInterface
{
    public:
        std::string const& pattern() const      = 0;
        std::string const& options() const      = 0;
};
#endif

template<typename T, char type, std::size_t size>
class FixedSizeStreamableObjectSerializer: public DefaultCustomSerializer<T>
{
    // Assumes T implements the DuckInterface "NormalSerializationInterface" (see above)
    // Note: You should pay special attention to the size (number of bytes these object should write).
    public:
        virtual char getBsonByteMark() const override                                                       {return type;}
        virtual std::size_t getPrintSizeBson(BsonPrinter& /*printer*/, T const& /*object*/) const override  {return size;}
        virtual void writeBson(BsonPrinter& printer, T const& object) const override                        {printer << object;}
        virtual void readBson(BsonParser& parser, char /*byteMarker*/, T& object) const override            {parser  >> object;}

        virtual void writeJson(JsonPrinter& printer, T const& object) const override                        {printer << object;}
        virtual void readJson(JsonParser& parser, T& object) const override                                 {parser >> object;}
};

// Specializations of FixedSizeStreamableObjectSerializer
template<typename T> class ObjectIDSerializer:  public FixedSizeStreamableObjectSerializer<T, '\x07', 12> {};
template<typename T> class DataTimeSerializer:  public FixedSizeStreamableObjectSerializer<T, '\x09', 8>  {};
template<typename T> class TimeStampSerializer: public FixedSizeStreamableObjectSerializer<T, '\x11', 8>{};

template<typename T, char encodeType = '\x08'>
class BinarySerializer: public DefaultCustomSerializer<T>
{
    // Assumes T implements the DuckInterface "DataInterface" (see above)
    public:
        virtual char getBsonByteMark() const override                                                       {return '\x05';}
        virtual std::size_t getPrintSizeBson(BsonPrinter& /*printer*/, T const& object) const override      {return 4 + 1 + object.getSize();}
        virtual void writeBson(BsonPrinter& printer, T const& object) const override
        {
            std::int32_t    size    = object.getSize();
            char            type    = encodeType;
            printer.writeLE<4, std::int32_t>(size);
            printer.stream().write(&type, 1);
            printer.stream().write(object.getBuffer(), object.getSize());
        }
        virtual void readBson(BsonParser& parser, char /*byteMarker*/, T& object) const override
        {
            std::int32_t    size    = parser.readLE<4, std::int32_t>();
            char            type;
            object.resize(size);
            parser.stream().read(&type, 1);
            parser.stream().read(object.getBuffer(), size);
        }
};

template<typename T>
class JavascriptSerializer: public DefaultCustomSerializer<T>
{
    // Assumes T implements the DuckInterface "DataInterface" (see above)
    public:
        virtual char getBsonByteMark() const override                                                   {return '\x0D';}
        virtual std::size_t getPrintSizeBson(BsonPrinter& /*printer*/, T const& object) const override  {return 4 + object.getSize() + 1;}
        virtual void writeBson(BsonPrinter& printer, T const& object) const override
        {
            std::int32_t    size = object.getSize();
            printer.writeLE<4, std::int32_t>(size + 1);
            printer.stream().write(object.getBuffer(), size);
            printer.stream().write("", 1);
        }
        virtual void readBson(BsonParser& parser, char /*byteMarker*/, T& object) const override
        {
            std::int32_t    size = parser.readLE<4, std::int32_t>();
            object.resize(size);
            parser.stream().read(object.getBuffer(), size);
            object.resize(size - 1);
        }
};

template<typename T>
class RegExSerializer: public DefaultCustomSerializer<T>
{
    // Assumes T implements the DuckInterface "RegExInterface" (see above)
    public:
        virtual char getBsonByteMark() const override                                                   {return '\x0B';}
        virtual std::size_t getPrintSizeBson(BsonPrinter& /*printer*/, T const& object) const override  {return object.pattern().size() + 1 + object.options().size() + 1;}
        virtual void writeBson(BsonPrinter& printer, T const& object) const override
        {
            printer.stream().write(object.pattern().data(), object.pattern().size());
            printer.stream().write("", 1);
            printer.stream().write(object.options().data(), object.options().size());
            printer.stream().write("", 1);
        }
        virtual void readBson(BsonParser& parser, char /*byteMarker*/, T& object) const override
        {
            std::getline(parser.stream(), object.pattern(), '\0');
            std::getline(parser.stream(), object.options(), '\0');
        }
};

class ObjectID
{
    // 4 byte timestamp
    // 5 byte random
    // 3 byte incrementing counter.
    static int classCounter; // = rand();

    std::int32_t    timestamp;
    std::int64_t    random;
    std::int32_t    counter;
    public:
        static int getNextCounter()
        {
            return (classCounter++) % 0xFFF;
        }
        ObjectID(std::int32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), std::int64_t random = std::rand(), std::int32_t counter = ObjectID::getNextCounter());
        bool operator==(ObjectID const& rhs) const {return std::tie(timestamp, random, counter) == std::tie(rhs.timestamp, rhs.random, rhs.counter);}
        bool operator<(ObjectID const& rhs)  const {return std::tie(timestamp, random, counter) <  std::tie(rhs.timestamp, rhs.random, rhs.counter);}
        friend BsonPrinter& operator<<(BsonPrinter& printer, ObjectID const& data);
        friend JsonPrinter& operator<<(JsonPrinter& printer, ObjectID const& data);
        friend BsonParser& operator>>(BsonParser& parser, ObjectID& data);
        friend JsonParser& operator>>(JsonParser& parser, ObjectID& data);
};
class UTCDateTime
{
    // Time in ms since the epoch
    std::int64_t    datetime;
    public:
        UTCDateTime(std::int64_t datetime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        bool operator==(UTCDateTime const& rhs) const {return datetime == rhs.datetime;}
        bool operator<(UTCDateTime const& rhs)  const {return datetime < rhs.datetime;}
        friend BsonPrinter& operator<<(BsonPrinter& printer, UTCDateTime const& data);
        friend JsonPrinter& operator<<(JsonPrinter& printer, UTCDateTime const& data);
        friend BsonParser& operator>>(BsonParser& parser, UTCDateTime& data);
        friend JsonParser& operator>>(JsonParser& parser, UTCDateTime& data);
};

class BsonTimeStamp
{
    // First 4 bytes are an increment
    // Second 4 are a timestamp (seconds since the epoch)
    std::int32_t    increment;
    std::int32_t    timestamp;
    public:
        BsonTimeStamp(std::time_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), int inc = 0);
        bool operator==(BsonTimeStamp const& rhs) const {return std::tie(increment, timestamp) == std::tie(rhs.increment, rhs.timestamp);}
        bool operator<(BsonTimeStamp const& rhs)  const {return std::tie(increment, timestamp) < std::tie(rhs.increment, rhs.timestamp);}
        UTCDateTime asDateTime();
        friend BsonPrinter& operator<<(BsonPrinter& printer, BsonTimeStamp const& data);
        friend JsonPrinter& operator<<(JsonPrinter& printer, BsonTimeStamp const& data);
        friend BsonParser& operator>>(BsonParser& parser, BsonTimeStamp& data);
        friend JsonParser& operator>>(JsonParser& parser, BsonTimeStamp& data);
};
        }
    }
}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Serialize::MongoUtility::ObjectID,       ThorsAnvil::Serialize::MongoUtility::ObjectIDSerializer<ThorsAnvil::Serialize::MongoUtility::ObjectID>);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Serialize::MongoUtility::UTCDateTime,    ThorsAnvil::Serialize::MongoUtility::DataTimeSerializer<ThorsAnvil::Serialize::MongoUtility::UTCDateTime>);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Serialize::MongoUtility::BsonTimeStamp,  ThorsAnvil::Serialize::MongoUtility::TimeStampSerializer<ThorsAnvil::Serialize::MongoUtility::BsonTimeStamp>);

#endif
