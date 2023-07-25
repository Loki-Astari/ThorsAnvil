#ifndef THORSANVIL_DB_MONGO_THORSMONGO_FIND_AND_MODIFY_H
#define THORSANVIL_DB_MONGO_THORSMONGO_FIND_AND_MODIFY_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

// Note: FARemoveConfig is similar to FAModifyConfig.
//       We have removed the fields upsert and returnNew
//       as these are not supported when doing a remove.
class FARemoveConfig: public ActionConfig<FARemoveConfig>
{
    std::optional<Sort>             sort;
    std::optional<Projection>       fields;
    std::optional<bool>             bypassDocumentValidation;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<Collation>        collation;
    // TODO
    // Array                        arrayFilters    // see: https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    std::optional<Hint>             hint;
    std::optional<std::string>      comment;
    std::optional<Let>              let;
    public:
        std::optional<Sort>             const getSort()                     const   {return sort;}
        std::optional<Projection>       const getFields()                   const   {return fields;}
        std::optional<bool>             const getBypassDocumentValidation() const   {return bypassDocumentValidation;}
        std::optional<WriteConcern>     const getWriteConcern()             const   {return writeConcern;}
        std::optional<std::uint32_t>    const getMaxTimeMS()                const   {return maxTimeMS;}
        std::optional<Collation>        const getCollation()                const   {return collation;}
        std::optional<Hint>             const getHint()                     const   {return hint;}
        std::optional<std::string>      const getComment()                  const   {return comment;}
        std::optional<Let>              const getLet()                      const   {return let;}

        FARemoveConfig& setSort(Sort value)                       {sort.emplace(value);                       return *this;}
        FARemoveConfig& setFields(Projection value)               {fields.emplace(value);                     return *this;}
        FARemoveConfig& setBypassDocumentValidation(bool value)   {bypassDocumentValidation.emplace(value);   return *this;}
        FARemoveConfig& setWriteConcern(WriteConcern value)       {writeConcern.emplace(value);               return *this;}
        FARemoveConfig& setMaxTimeMS(std::uint32_t value)         {maxTimeMS.emplace(value);                  return *this;}
        FARemoveConfig& setCollation(Collation value)             {collation.emplace(value);                  return *this;}
        FARemoveConfig& setHint(Hint value)                       {hint.emplace(value);                       return *this;}
        FARemoveConfig& setComment(std::string value)             {comment.emplace(value);                    return *this;}
        FARemoveConfig& setLet(Let value)                         {let.emplace(value);                        return *this;}
};

class FAModifyConfig: public ActionConfig<FAModifyConfig>
{
    std::optional<Sort>             sort;
    std::optional<bool>             returnNew;  /* actual new */
    std::optional<Projection>       fields;
    std::optional<bool>             upsert;
    std::optional<bool>             bypassDocumentValidation;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<Collation>        collation;
    // TODO
    // Array                        arrayFilters    // see: https://www.mongodb.com/docs/manual/reference/command/findAndModify/
    std::optional<Hint>             hint;
    std::optional<std::string>      comment;
    std::optional<Let>              let;
    public:
        std::optional<Sort>             const getSort()                     const   {return sort;}
        std::optional<bool>             const getReturnNew()                const   {return returnNew;}  /* actual new */
        std::optional<Projection>       const getFields()                   const   {return fields;}
        std::optional<bool>             const getUpsert()                   const   {return upsert;}
        std::optional<bool>             const getBypassDocumentValidation() const   {return bypassDocumentValidation;}
        std::optional<WriteConcern>     const getWriteConcern()             const   {return writeConcern;}
        std::optional<std::uint32_t>    const getMaxTimeMS()                const   {return maxTimeMS;}
        std::optional<Collation>        const getCollation()                const   {return collation;}
        std::optional<Hint>             const getHint()                     const   {return hint;}
        std::optional<std::string>      const getComment()                  const   {return comment;}
        std::optional<Let>              const getLet()                      const   {return let;}

        FAModifyConfig& setSort(Sort value)                       {sort.emplace(value);                       return *this;}
        FAModifyConfig& setReturnNew(bool value)                  {returnNew.emplace(value);                  return *this;}  /* actual new */
        FAModifyConfig& setFields(Projection value)               {fields.emplace(value);                     return *this;}
        FAModifyConfig& setUpsert(bool value)                     {upsert.emplace(value);                     return *this;}
        FAModifyConfig& setBypassDocumentValidation(bool value)   {bypassDocumentValidation.emplace(value);   return *this;}
        FAModifyConfig& setWriteConcern(WriteConcern value)       {writeConcern.emplace(value);               return *this;}
        FAModifyConfig& setMaxTimeMS(std::uint32_t value)         {maxTimeMS.emplace(value);                  return *this;}
        FAModifyConfig& setCollation(Collation value)             {collation.emplace(value);                  return *this;}
        FAModifyConfig& setHint(Hint value)                       {hint.emplace(value);                       return *this;}
        FAModifyConfig& setComment(std::string value)             {comment.emplace(value);                    return *this;}
        FAModifyConfig& setLet(Let value)                         {let.emplace(value);                        return *this;}
};

template<typename T>
struct FAModifyResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<FAModifyResult<T>>
{
    std::unique_ptr<T>  value;
    LastErrorObject     lastErrorObject;
};

}

ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::FAModifyResult,            value, lastErrorObject);

#endif
