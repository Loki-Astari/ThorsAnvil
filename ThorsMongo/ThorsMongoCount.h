#ifndef THORSANVIL_DB_MONGO_THORSMONGO_COUNT_H
#define THORSANVIL_DB_MONGO_THORSMONGO_COUNT_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class CountConfig: public ActionConfig<CountConfig>
{
    std::optional<std::uint32_t>    limit;                  // Default no-limit (0 => no-limit)
    std::optional<std::uint32_t>    skip;                   // Default 0
    std::optional<Hint>             hint;
    std::optional<ReadConcern>      readConcern;
    std::optional<std::uint32_t>    maxTimeMS;              // Default no-timeout  0 => no-timeout
    std::optional<Collation>        collation;
    std::optional<std::string>      comment;

    public:
        std::optional<std::uint32_t>    const& getLimit()              const {return limit;}
        std::optional<std::uint32_t>    const& getSkip()               const {return skip;}
        std::optional<Hint>             const& getHint()               const {return hint;}
        std::optional<ReadConcern>      const& getReadConcern()        const {return readConcern;}
        std::optional<std::uint32_t>    const& getMaxTimeMS()          const {return maxTimeMS;}
        std::optional<Collation>        const& getCollation()          const {return collation;}
        std::optional<std::string>      const& getComment()            const {return comment;}

        CountConfig&     setLimit(std::uint32_t value)       {limit.emplace(value);return *this;}
        CountConfig&     setSkip(std::uint32_t value)        {skip.emplace(value);return *this;}
        CountConfig&     setHint(Hint value)                 {hint.emplace(value);return *this;}
        CountConfig&     setReadConcern(ReadConcern value)   {readConcern.emplace(value);return *this;}
        CountConfig&     setMaxTimeMS(std::uint32_t value)   {maxTimeMS.emplace(value);return *this;}
        CountConfig&     setCollation(Collation value)       {collation.emplace(value);return *this;}
        CountConfig&     setComment(std::string value)       {comment.emplace(value);return *this;}
};


struct CountResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<CountResult>
{
    std::uint64_t   n;
};

}

ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::CountResult,             n);

#endif
