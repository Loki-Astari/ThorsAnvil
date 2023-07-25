#ifndef THORSANVIL_DB_MONGO_THORSMONGO_REMOVE_H
#define THORSANVIL_DB_MONGO_THORSMONGO_REMOVE_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class RemoveConfig: public ActionConfig<RemoveConfig>
{
    std::optional<std::string>      comment;
    std::optional<Let>              let;
    std::optional<bool>             ordered;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::uint32_t>    maxTimeMS;

    public:
        std::optional<std::string>      const& getComment()            const   {return comment;}
        std::optional<Let>              const& getLet()                const   {return let;}
        std::optional<bool>             const& getOrdered()            const   {return ordered;}
        std::optional<WriteConcern>     const& getWriteConcern()       const   {return writeConcern;}
        std::optional<std::uint32_t>    const& getMaxTimeMS()          const   {return maxTimeMS;}

        RemoveConfig& setComment(std::string value)         {comment.emplace(value);            return *this;}
        RemoveConfig& setLet(Let value)                     {let.emplace(value);                return *this;}
        RemoveConfig& setOrdered(bool value)                {ordered.emplace(value);            return *this;}
        RemoveConfig& setWriteConcern(WriteConcern value)   {writeConcern.emplace(value);       return *this;}
        RemoveConfig& setMaxTimeMS(std::uint32_t value)     {maxTimeMS.emplace(value);          return *this;}
};

struct RemoveResult: public ModifyResult, public MongoActionReadInterfaceTrivialImpl<RemoveResult>
{};

}

ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::ModifyResult,
                        ThorsAnvil::DB::Mongo::RemoveResult);

#endif
