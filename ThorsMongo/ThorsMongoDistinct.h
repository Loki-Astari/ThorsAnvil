#ifndef THORSANVIL_DB_MONGO_THORSMONGO_DISTINCT_H
#define THORSANVIL_DB_MONGO_THORSMONGO_DISTINCT_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class DistinctConfig: public ActionConfig<DistinctConfig>
{
    std::optional<ReadConcern>      readConcern;
    std::optional<Collation>        collation;
    std::optional<std::string>      comment;

    public:
        std::optional<ReadConcern>      const& getReadConcern()        const {return readConcern;}
        std::optional<Collation>        const& getCollation()          const {return collation;}
        std::optional<std::string>      const& getComment()            const {return comment;}

        DistinctConfig&     setReadConcern(ReadConcern value)   {readConcern.emplace(value);return *this;}
        DistinctConfig&     setCollation(Collation value)       {collation.emplace(value);return *this;}
        DistinctConfig&     setComment(std::string value)       {comment.emplace(value);return *this;}
};

template<typename T>
struct DistinctResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<DistinctResult<T>>
{
    std::vector<T>   values;
};

}

ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::DistinctResult,             values);

#endif
