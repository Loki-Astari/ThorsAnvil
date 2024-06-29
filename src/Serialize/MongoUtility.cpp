#include "MongoUtility.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <tuple>
#ifndef __WINNT
#include <arpa/inet.h>
#endif

using namespace ThorsAnvil::Serialize;
using namespace ThorsAnvil::Serialize::MongoUtility;

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
int& ObjectID::classCounter()
{
    static int classCounterStoreage = 32769 + std::rand();
    return classCounterStoreage;
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
ObjectID::ObjectID(std::int32_t timestamp, std::int64_t random, std::int32_t counter)
    : timestamp(timestamp)
    , random(random)
    , counter(counter)
{}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
UTCDateTime::UTCDateTime(std::int64_t datetime)
    : datetime(datetime)
{}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
BsonTimeStamp::BsonTimeStamp(std::time_t timestamp, int inc)
    : increment(inc)
    , timestamp(timestamp)
{}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
UTCDateTime BsonTimeStamp::asDateTime()
{
    return UTCDateTime(timestamp * 1000);
}
