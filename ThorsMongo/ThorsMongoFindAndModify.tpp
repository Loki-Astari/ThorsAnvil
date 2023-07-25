#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename F, typename U>
class FindAndModify: public MongoActionWriteInterfaceTrivialImpl<FindAndModify<F, U>>
{
    friend class ThorsAnvil::Serialize::Traits<FindAndModify<F, U>>;
    std::string                     findAndModify;
    std::string                     $db;
    F                               query;
    std::optional<U>                update;
    std::optional<bool>             remove;
    std::optional<Sort>             sort;
    std::optional<bool>             returnNew;
    std::optional<Projection>       fields;
    std::optional<bool>             upsert;
    std::optional<bool>             bypassDocumentValidation;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<Collation>        collation;
    std::optional<Hint>             hint;
    std::optional<std::string>      comment;
    std::optional<Let>              let;

    public:
        FindAndModify(std::string_view collection, std::string_view db, FAModifyConfig const& config, F const& query, std::optional<U> const& update)
            : findAndModify(collection)
            , $db(db)
            , query(query)
            , update(update)
            , remove(std::optional<bool>{})
            , sort(config.getSort())
            , returnNew(config.getReturnNew())
            , fields(config.getFields())
            , upsert(config.getUpsert())
            , bypassDocumentValidation(config.getBypassDocumentValidation())
            , writeConcern(config.getWriteConcern())
            , maxTimeMS(config.getMaxTimeMS())
            , collation(config.getCollation())
            , hint(config.getHint())
            , comment(config.getComment())
            , let(config.getLet())
        {}
        FindAndModify(std::string_view collection, std::string_view db, FARemoveConfig const& config, F const& query)
            : findAndModify(collection)
            , $db(db)
            , query(query)
            , update(std::optional<bool>{})
            , remove({true})
            , sort(config.getSort())
            , fields(config.getFields())
            , bypassDocumentValidation(config.getBypassDocumentValidation())
            , writeConcern(config.getWriteConcern())
            , maxTimeMS(config.getMaxTimeMS())
            , collation(config.getCollation())
            , hint(config.getHint())
            , comment(config.getComment())
            , let(config.getLet())
        {}
};

}

template<typename T, typename F>
inline
FAModifyResult<T> Collection::findAndReplaceOne(F const& search, T const& replace, FAModifyConfig const& config)
{
    FAModifyResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::FindAndModify<F, T>{colName(), dbName(), config, search, replace},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(response, messageId, config.getParserConfig()))
        {
            // The message was sent and reply read correctly.
            // Note: This does not mean it worked.
            //       There may be an error message in the response object
        }
    }
    return response;
}

template<typename T, typename F>
inline
FAModifyResult<T> Collection::findAndRemoveOne(F const& search, FARemoveConfig const& config)
{
    FAModifyResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::FindAndModify<F, bool>{colName(), dbName(), config, search},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(response, messageId, config.getParserConfig()))
        {
            // The message was sent and reply read correctly.
            // Note: This does not mean it worked.
            //       There may be an error message in the response object
        }
    }
    return response;
}

template<typename T, typename F, typename U>
inline
FAModifyResult<T> Collection::findAndUpdateOne(F const& search, U const& update, FAModifyConfig const& config)
{
    FAModifyResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::FindAndModify<F, U>{colName(), dbName(), config, search, update},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(response, messageId, config.getParserConfig()))
        {
            // The message was sent and reply read correctly.
            // Note: This does not mean it worked.
            //       There may be an error message in the response object
        }
    }
    return response;
}

template<typename T, typename F, typename... U>
inline
FAModifyResult<T> Collection::findAndUpdateOne(F const& search, std::tuple<U const&...> update, FAModifyConfig const& config)
{
    FAModifyResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::FindAndModify<F, std::tuple<U const&...>>{colName(), dbName(), config, search, update},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(response, messageId, config.getParserConfig()))
        {
            // The message was sent and reply read correctly.
            // Note: This does not mean it worked.
            //       There may be an error message in the response object
        }
    }
    return response;
}
}

ThorsAnvil_Template_MakeOverride(2,
                        ThorsAnvil::DB::Mongo::Action::FindAndModify, {"returnNew", "new"});
ThorsAnvil_Template_MakeTrait(2,
                        ThorsAnvil::DB::Mongo::Action::FindAndModify,       findAndModify, $db, query, update, remove, sort,
                                                                            returnNew, fields, upsert, bypassDocumentValidation,
                                                                            writeConcern, maxTimeMS, collation, hint, comment,
                                                                            let);
