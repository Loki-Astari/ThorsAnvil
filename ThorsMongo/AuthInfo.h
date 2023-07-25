#ifndef THORSANVIL_DB_MONGO_AUTH_INFO_H
#define THORSANVIL_DB_MONGO_AUTH_INFO_H

#include "ThorsMongoConfig.h"

#include <string>

namespace ThorsAnvil::DB::Mongo::Auth
{

struct UserNamePassword
{
    std::string     username;
    std::string     password;
    std::string     database;
};

struct Certificate
{
};


}

#endif
