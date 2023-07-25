#ifndef THORSANVIL_DB_MONGO_THORSMONGO_GETMORE_H
#define THORSANVIL_DB_MONGO_THORSMONGO_GETMORE_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class GetMoreConfig: public ActionConfig<GetMoreConfig>
{
    std::optional<std::uint32_t>    batchSize;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<std::string>      comment;
    public:
        std::optional<std::uint32_t>    const& getBatchSize()          const   {return batchSize;}
        std::optional<std::uint32_t>    const& getMaxTimeMS()          const   {return maxTimeMS;}
        std::optional<std::string>      const& getComment()            const   {return comment;}

        GetMoreConfig&  setBatchSize(std::uint32_t value)   {batchSize.emplace(value);return *this;}
        GetMoreConfig&  setMaxTimeMS(std::uint32_t value)   {maxTimeMS.emplace(value);return *this;}
        GetMoreConfig&  setComment(std::string value)       {comment.emplace(value);return *this;}
};

// See in MongoCursor.h
template<typename T>
class CursorNext;

template<typename T>
class GetMoreResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<GetMoreResult<T>>
{
    public:
        CursorNext<T>               cursor;
        TimeStamp                   operationTime;
        ClusterTime                 $clusterTime;
};

}

ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::GetMoreResult,           cursor, operationTime, $clusterTime);

#endif
