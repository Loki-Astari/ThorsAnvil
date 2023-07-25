#ifndef THORSANVIL_DB_MONGO_THORSMONGO_INSERT_H
#define THORSANVIL_DB_MONGO_THORSMONGO_INSERT_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class InsertConfig: public ActionConfig<InsertConfig>
{
    std::optional<bool>             ordered;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<WriteConcern>     writeConcern;
    std::optional<bool>             bypassDocumentValidation;
    std::optional<std::string>      comment;

    public:
        std::optional<bool>             const& getOrdered()            const   {return ordered;}
        std::optional<std::uint32_t>    const& getMaxTimeMS()          const   {return maxTimeMS;}
        std::optional<WriteConcern>     const& getWriteConcern()       const   {return writeConcern;}
        std::optional<bool>             const& getBypassDocumentValidation()   const   {return bypassDocumentValidation;}
        std::optional<std::string>      const& getComment()            const   {return comment;}

        InsertConfig& setOrdered(bool value)                    {ordered.emplace(value);                    return *this;}
        InsertConfig& setMaxTimeMS(std::uint32_t value)         {maxTimeMS.emplace(value);                  return *this;}
        InsertConfig& setWriteConcern(WriteConcern value)       {writeConcern.emplace(value);               return *this;}
        InsertConfig& setBypassDocumentValidation(bool value)   {bypassDocumentValidation.emplace(value);   return *this;}
        InsertConfig& setComment(std::string value)             {comment.emplace(value);                    return *this;}
};

struct InsertResult: public ModifyResult, public MongoActionReadInterfaceTrivialImpl<InsertResult>
{
    // List of ObjectID that were sent for insertion.
    // Need to check the "writeErrors" to validate that they were inserted.
    std::vector<ObjectID>       inserted;
};

}

ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::ModifyResult,
                        ThorsAnvil::DB::Mongo::InsertResult);

#endif
