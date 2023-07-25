#ifndef THORSANVIL_DB_MONGO_THORSMONGO_COMMADN_CONFIG_H
#define THORSANVIL_DB_MONGO_THORSMONGO_COMMADN_CONFIG_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class CommandConfig: public ActionConfig<CommandConfig>
{
    std::optional<bool>             nameOnly;
    std::optional<bool>             authorizedCollections;
    std::optional<std::string>      comment;

    public:
        std::optional<bool>             const& getNameOnly()                const {return nameOnly;}
        std::optional<bool>             const& getAuthorizedCollections()   const {return authorizedCollections;}
        std::optional<std::string>      const& getComment()                 const {return comment;}

        CommandConfig&     setNameOnly(bool value)               {nameOnly.emplace(value);return *this;}
        CommandConfig&     setAuthorizedCollections(bool value)  {authorizedCollections.emplace(value);return *this;}
        CommandConfig&     setComment(std::string value)         {comment.emplace(value);return *this;}
};

}

#endif
