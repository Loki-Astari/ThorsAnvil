#ifndef THORSANVIL_SERIALIZE_MONGO_UTILITY_OBJECT_ID_H
#define THORSANVIL_SERIALIZE_MONGO_UTILITY_OBJECT_ID_H

#include "SerializeConfig.h"

#include <cstdint>
#include <chrono>
#include <tuple>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

namespace ThorsAnvil::Serialize
{

class BsonPrinter;
class BsonParser;
class JsonPrinter;
class JsonParser;

namespace MongoUtility
{

class ObjectID
{
    // 4 byte timestamp
    // 5 byte random
    // 3 byte incrementing counter.
    static int& classCounter();

    std::int32_t    timestamp;
    std::int64_t    random;
    std::int32_t    counter;

    mutable std::string     str;

    public:
        static int getNextCounter()
        {
            return (classCounter()++) % 0xFFF;
        }
        std::string const& to_string() const
        {
            if (str == "") {
                std::stringstream stream;
                stream << (*this);
                str = stream.str();
            }
            return str;
        }
        ObjectID(std::int32_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), std::int64_t random = std::rand(), std::int32_t counter = ObjectID::getNextCounter());
        ObjectID(std::string const& idStr);
        bool operator==(ObjectID const& rhs) const {return std::tie(timestamp, random, counter) == std::tie(rhs.timestamp, rhs.random, rhs.counter);}
        bool operator<(ObjectID const& rhs)  const {return std::tie(timestamp, random, counter) <  std::tie(rhs.timestamp, rhs.random, rhs.counter);}
        friend BsonPrinter& operator<<(BsonPrinter& printer, ObjectID const& data);
        friend JsonPrinter& operator<<(JsonPrinter& printer, ObjectID const& data);
        friend std::ostream& operator<<(std::ostream& stream, ObjectID const& data);
        friend BsonParser& operator>>(BsonParser& parser, ObjectID& data);
        friend JsonParser& operator>>(JsonParser& parser, ObjectID& data);
        friend std::istream& operator>>(std::istream& stream, ObjectID& data);
};

}
}

#endif
