#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename F>
class Counter: public MongoActionWriteInterfaceTrivialImpl<Counter<F>>
{
    friend class ThorsAnvil::Serialize::Traits<Counter<F>>;

    std::string_view                count;
    std::string_view                $db;
    F                               query;
    std::optional<std::uint32_t>    limit;                  // Default no-limit (0 => no-limit)
    std::optional<std::uint32_t>    skip;                   // Default 0
    std::optional<Hint>             hint;
    std::optional<ReadConcern>      readConcern;
    std::optional<std::uint32_t>    maxTimeMS;              // Default no-timeout  0 => no-timeout
    std::optional<Collation>        collation;
    std::optional<std::string>      comment;

    public:
        Counter(std::string_view count, std::string_view db, CountConfig const& config, F query)
            : count(count)
            , $db(db)
            , query(query)
            , limit(config.getLimit())
            , skip(config.getSkip())
            , hint(config.getHint())
            , readConcern(config.getReadConcern())
            , maxTimeMS(config.getMaxTimeMS())
            , collation(config.getCollation())
            , comment(config.getComment())
        {}
};

}


template<typename F>
inline
CountResult Collection::countDocuments(F const& query, CountConfig const& config)
{
    CountResult     response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Counter<F>{colName(), dbName(), config, query},
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
inline
CountResult Collection::countDocuments(CountConfig const& config)
{
    CountResult     response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Counter<All>{colName(), dbName(), config, All{}},
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

ThorsAnvil_Template_MakeTrait(1,
                        ThorsAnvil::DB::Mongo::Action::Counter,         count, $db, query, limit, skip,
                                                                        hint, readConcern, maxTimeMS,
                                                                        collation, comment);
