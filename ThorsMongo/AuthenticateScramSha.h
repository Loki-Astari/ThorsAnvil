#ifndef THORSANVIL_DB_MONGO_AUTHENTICATE_SCRAM_SHA_H
#define THORSANVIL_DB_MONGO_AUTHENTICATE_SCRAM_SHA_H

// https://github.com/mongodb/specifications/blob/master/source/mongodb-handshake/handshake.rst

#include "ThorsMongoConfig.h"
#include "MongoUtil.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "MessageHandler.h"
#include "BinaryData.h"

#include "ThorSerialize/Traits.h"

#include <string>
#include <cstdint>
#include <cstddef>

#if !defined(__WINNT__)
#include <sys/utsname.h>
#endif

namespace ThorsAnvil::DB::Mongo::Auth::ScramSha256
{

struct AuthInit: public MongoActionWriteInterfaceTrivialImpl<AuthInit>
{
    AuthInit(std::string const& db, std::string const& mechanism, std::string&& payload);

    std::int32_t        saslStart;
    std::string         mechanism;
    std::string         $db;
    Internal::Binary    payload;
};

struct AuthCont: public MongoActionWriteInterfaceTrivialImpl<AuthCont>
{
    AuthCont(std::int32_t convId, std::string const& db, std::string&& payload);

    std::int32_t        saslContinue;
    std::int32_t        conversationId;
    std::string         $db;
    Internal::Binary    payload;
};

struct AuthReply: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<AuthReply>
{
    AuthReply();

    std::int32_t        conversationId;     // Seen
    bool                done;               // Seen
    Internal::Binary    payload;            // Seen
};

void authenticate(MessageHandler& messageHandler, Auth::UserNamePassword const& authInfo);

}

// Traits classes used to define how objects are serialized.
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthInit,            saslStart, mechanism, payload, $db);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthCont,            saslContinue, payload, conversationId, $db);
ThorsAnvil_ExpandTrait(ThorsAnvil::DB::Mongo::CmdReplyBase,
                     ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthReply,           conversationId, done, payload);

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "AuthenticateScramSha.source"
#endif

#endif
