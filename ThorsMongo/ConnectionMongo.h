#ifndef THORSANVIL_DB_MONGO_CONNECTION_MONGO_H
#define THORSANVIL_DB_MONGO_CONNECTION_MONGO_H

#include "ThorsMongoConfig.h"
#include "ConnectionBufferMongo.h"
#include "ThorsSocket/SocketStream.h"

#include <string>

namespace ThorsAnvil::DB::Mongo
{

struct MongoURL
{
    std::string     hostname;
    int             port        = 27017;
};

class ConnectionMongo: public ThorsSocket::BaseSocketStream<ConnectionBufferMongo>
{
    public:
        ConnectionMongo(MongoURL const& url);
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ConnectionMongo.source"
#endif

#endif
