#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename F>
class Distinct: public MongoActionWriteInterfaceTrivialImpl<Distinct<F>>
{
    friend class ThorsAnvil::Serialize::Traits<Distinct<F>>;

    std::string_view                distinct;
    std::string_view                $db;
    std::string                     key;
    std::optional<F>                query;
    std::optional<ReadConcern>      readConcern;
    std::optional<Collation>        collation;
    std::optional<std::string>      comment;

    public:
        Distinct(std::string_view count, std::string_view db, DistinctConfig const& config, std::string const& key, F query)
            : distinct(count)
            , $db(db)
            , key(key)
            , query(query)
            , readConcern(config.getReadConcern())
            , collation(config.getCollation())
            , comment(config.getComment())
        {}
};

}


template<typename T, typename F>
inline
DistinctResult<T> Collection::distinct(std::string const& key, F const& query, DistinctConfig const& config)
{
    DistinctResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Distinct<F>{colName(), dbName(), config, key, query},
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
        else if (config.getServerErrorAreExceptions())
        {
            throw response;
        }
    }
    return response;
}

template<typename T>
inline
DistinctResult<T> Collection::distinct(std::string const& key, DistinctConfig const& config)
{
    DistinctResult<T>   response;
    MessageId           messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Distinct<All>{colName(), dbName(), config, key, All{}},
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
        else if (config.getServerErrorAreExceptions())
        {
            throw response;
        }
    }
    return response;
}

}

ThorsAnvil_Template_MakeTrait(1,
                        ThorsAnvil::DB::Mongo::Action::Distinct,        distinct, $db, key, query,
                                                                        readConcern, collation, comment);
