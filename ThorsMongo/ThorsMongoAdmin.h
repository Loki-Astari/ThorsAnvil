#ifndef THORSANVIL_DB_MONGO_THORSMONGO_ADMIN_H
#define THORSANVIL_DB_MONGO_THORSMONGO_ADMIN_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"


namespace ThorsAnvil::DB::Mongo
{

class RenameConfig: public ActionConfig<RenameConfig>
{
    std::optional<bool>             dropTarget;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        std::optional<bool>         const& getDropTarget()      const {return dropTarget;}
        std::optional<WriteConcern> const& getWriteConcern()    const {return writeConcern;}
        std::optional<std::string>  const& getComment()         const {return comment;}

        RenameConfig&   setDropTarget(bool value)                   {dropTarget.emplace(value);     return *this;}
        RenameConfig&   setWriteConcern(WriteConcern const& value)  {writeConcern.emplace(value);   return *this;}
        RenameConfig&   setComment(std::string const& value)        {comment.emplace(value);        return *this;}
};

class DropCollectionConfig: public ActionConfig<DropCollectionConfig>
{
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        std::optional<WriteConcern> const& getWriteConcern()    const {return writeConcern;}
        std::optional<std::string>  const& getComment()         const {return comment;}

        DropCollectionConfig&   setWriteConcern(WriteConcern const& value)  {writeConcern.emplace(value);   return *this;}
        DropCollectionConfig&   setComment(std::string const& value)        {comment.emplace(value);        return *this;}
};

class CreateCollectionConfig: public ActionConfig<CreateCollectionConfig>
{
    std::optional<bool>             capped;
    std::optional<std::uint32_t>    expireAfterSeconds;
    std::optional<std::uint64_t>    size;
    std::optional<std::uint64_t>    maxSize;
    std::optional<ValidationLevel>  validationLevel;
    std::optional<Collation>        collation;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        std::optional<bool>             const& getCapped()              const {return capped;}
        std::optional<std::uint32_t>    const& getExpireAfterSeconds()  const {return expireAfterSeconds;}
        std::optional<std::uint64_t>    const& getSize()                const {return size;}
        std::optional<std::uint64_t>    const& getMaxSize()             const {return maxSize;}
        std::optional<ValidationLevel>  const& getValidationLevel()     const {return validationLevel;}
        std::optional<Collation>        const& getCollation()           const {return collation;}
        std::optional<WriteConcern>     const& getWriteConcern()        const {return writeConcern;}
        std::optional<std::string>      const& getComment()             const {return comment;}

        CreateCollectionConfig&   setCapped(bool value)                       {capped.emplace(value);               return *this;}
        CreateCollectionConfig&   setExpireAfterSeconds(std::int32_t value)   {expireAfterSeconds.emplace(value);   return *this;}
        CreateCollectionConfig&   setSize(std::int64_t value)                 {size.emplace(value);                 return *this;}
        CreateCollectionConfig&   setMaxSize(std::int64_t value)              {maxSize.emplace(value);              return *this;}
        CreateCollectionConfig&   setValidationLevel(ValidationLevel value)   {validationLevel.emplace(value);      return *this;}
        CreateCollectionConfig&   setCollation(Collation value)               {collation.emplace(value);            return *this;}
        CreateCollectionConfig&   setWriteConcern(WriteConcern const& value)  {writeConcern.emplace(value);         return *this;}
        CreateCollectionConfig&   setComment(std::string const& value)        {comment.emplace(value);              return *this;}
};

struct AdminResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<AdminResult>
{};


}

ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::AdminResult);

#endif
