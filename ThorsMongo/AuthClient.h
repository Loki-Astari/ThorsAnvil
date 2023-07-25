#ifndef THORSANVIL_DB_MONGO_AUTH_CLIENT_H
#define THORSANVIL_DB_MONGO_AUTH_CLIENT_H

// https://github.com/mongodb/specifications/blob/master/source/mongodb-handshake/handshake.rst

#include "ThorsMongoConfig.h"
#include "MongoUtil.h"

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace ThorsAnvil::DB::Mongo::Auth
{

/* The following classes are needed as part of the HandShake.
 * They describe the local application to the Mongo server.
 *
 * These classes don't do any work (i.e. they are property bags)
 * so they are implemented as "struct" not "class".
 *
 *  Driver:
 *  OS
 *  Application
 *  Client
 *
 * This class provides information about the Mongo server as part of the HandShakeReply.
 *
 *  Version
 *
 * Useful classes for Mongo
 *
 *  ObjecID
 *  UTCDateTime
 *
 *  Handshake:      This is the command sent to Mongo to initiate authentication.
 *                  The response indicates authentication methods available.
 *
 *  HandShakeReply: This is the object returned by Mongo.
 *                  It will contain the information required to start authentication.
 *
 *  Authenticator:  Defines the shape of the authenticator routes (like AuthenticatorScramSha)
 *  Authenticate:   A class that holds registered "Authenticator" functions.
 *                  The "authenticate()" method does the hand shake then uses an appropriate
 *                  authenticator to finish the job.
 *
 */
struct Driver
{
    std::string     name;
    std::string     version;

    Driver();
};

struct OS
{
    std::string                 type;
    std::optional<std::string>  name;
    std::optional<std::string>  architecture;
    std::optional<std::string>  version;

    OS();
};

struct Application
{
    std::string     name;

    Application(std::string const& application);
};

struct Client
{
    Application                 application;
    Driver                      driver;
    OS                          os;
    std::optional<std::string>  platform;

    Client(std::string const& application, std::optional<std::string> const& platform = {});
};

struct Version
{
    ObjectID            processId;                          // Seen
    std::int64_t        counter;                            // Seen
};

}

// Simple Triats classes to serialize objects into BSON.
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::Driver,             name, version);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::OS,                 type, name, architecture, version);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::Application,        name);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::Client,             application, driver, os);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::Auth::Version,            processId, counter);

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "AuthClient.source"
#endif

#endif
