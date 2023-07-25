#ifndef THORSANVIL_DB_MONGO_AUTHENTICATE_H
#define THORSANVIL_DB_MONGO_AUTHENTICATE_H

// https://github.com/mongodb/specifications/blob/master/source/mongodb-handshake/handshake.rst

#include "ThorsMongoConfig.h"
#include "MongoUtil.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "MessageHandler.h"

#include "ThorSerialize/MongoUtility.h"

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace ThorsAnvil::DB::Mongo
{

namespace Auth
{

struct HandShake: public MongoActionWriteInterfaceTrivialImpl<HandShake>
{
    using Compression = std::vector<std::string>;

    friend ThorsAnvil::Serialize::Traits<HandShake>;
    int             hello;
    std::string     saslSupportedMechs;
    std::string     $db;
    std::string     hostInfo;
    Client          client;
    Compression     compression;

    public:
        HandShake(std::string const& user, std::string const& db, Auth::Client const& client, std::vector<std::string> const& compression = {});
};

struct HandShakeReply: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<HandShakeReply>
{
    using UserData = void;

    Version         topologyVersion;                        // Seen
    UTCDateTime     localTime;                              // Seen
    std::int32_t    maxBsonObjectSize;                      // Seen
    std::int32_t    maxMessageSizeBytes;                    // Seen
    std::int32_t    maxWriteBatchSize;                      // Seen
    std::int32_t    logicalSessionTimeoutMinutes;           // Seen
    std::int32_t    connectionId;                           // Seen
    std::int32_t    minWireVersion;                         // Seen
    std::int32_t    maxWireVersion;                         // Seen
    bool            ismaster;
    bool            readOnly;                               // Seen
    bool            isWritablePrimary;                      // Seen
    std::vector<std::string>    saslSupportedMechs;         // Seen
    std::vector<std::string>    compression;
};

}

using Authenticator = std::function<void(MessageHandler&, Auth::UserNamePassword const&)>;
using AuthMap = std::map<std::string, Authenticator>;

class Authenticate
{
    private:
        AuthMap     authenticators;
    public:
        template<typename F>
        void addAuthenticator(std::string const& name, F&& auth)    {authenticators[name] = std::move(auth);}

        void handShake(MessageHandler& messageHandler,
                       Auth::UserNamePassword const& authInfo,
                       Compression useCompression,
                       Auth::Client const& clientInfo);
    private:
        void authenticteHandShake(MessageHandler& messageHandler,
                               Auth::UserNamePassword const& authInfo,
                               Auth::Client const& clientInfo,
                               std::vector<std::string> const& compression);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::HandShake,          hello, saslSupportedMechs, $db, hostInfo, client, compression);
ThorsAnvil_ExpandTrait(ThorsAnvil::DB::Mongo::CmdReplyBase,
                     ThorsAnvil::DB::Mongo::Auth::HandShakeReply,     topologyVersion, localTime, maxBsonObjectSize, maxMessageSizeBytes, maxWriteBatchSize, logicalSessionTimeoutMinutes, connectionId, minWireVersion, maxWireVersion, ismaster, readOnly, isWritablePrimary, saslSupportedMechs, compression);

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "Authenticate.source"
#endif

#endif
