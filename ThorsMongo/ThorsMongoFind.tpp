#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename T, typename F>
class Finder: public MongoActionWriteInterfaceTrivialImpl<Finder<T, F>>
{
    friend class ThorsAnvil::Serialize::Traits<Finder<T, F>>;
    std::string_view                find;
    std::string_view                $db;
    std::optional<F>                filter;
    std::optional<Sort>             sort;
    std::optional<Projection>       projection;
    std::optional<Hint>             hint;
    std::optional<std::uint32_t>    skip;                   // Default 0
    std::optional<std::uint32_t>    limit;                  // Default no-limit (0 => no-limit)
    std::optional<std::uint32_t>    batchSize;              // No Default. 0 => no values returned on initial call.
    std::optional<bool>             singleBatch;            // Default false
    std::optional<std::string>      comment;
    std::optional<std::uint32_t>    maxTimeMS;              // Default no-timeout  0 => no-timeout
    std::optional<ReadConcern>      readConcern;
    std::optional<Limit>            maxV;                   // Using max requires hint.
    std::optional<Limit>            minV;                   // Using min requires hint.
    std::optional<bool>             returnKey;              // Default false
    std::optional<bool>             showRecordId;
    std::optional<bool>             tailable;
    std::optional<bool>             awaitData;
    std::optional<bool>             noCursorTimeout;
    std::optional<bool>             allowPartialResults;
    std::optional<Collation>        collation;
    std::optional<bool>             allowDiskUse;
    std::optional<Let>              let;

    public:
        Finder(std::string_view find, std::string_view db, FindConfig const& config, std::optional<F> filter)
            : find(find)
            , $db(db)
            , filter(filter)
            , sort(config.getSort())
            , projection(config.getProjection())
            , hint(config.getHint())
            , skip(config.getSkip())
            , limit(config.getLimit())
            , batchSize(config.getBatchSize())
            , singleBatch(config.getSingleBatch())
            , comment(config.getComment())
            , maxTimeMS(config.getMaxTimeMS())
            , readConcern(config.getReadConcern())
            , maxV(config.getMax())
            , minV(config.getMin())
            , returnKey(config.getReturnKey())
            , showRecordId(config.getShowRecordId())
            , tailable(config.getTailable())
            , awaitData(config.getAwaitData())
            , noCursorTimeout(config.getNoCursorTimeout())
            , allowPartialResults(config.getAllowPartialResults())
            , collation(config.getCollation())
            , allowDiskUse(config.getAllowDiskUse())
            , let(config.getLet())

        {}
};

}


template<typename T>
inline
FindRange<T> Collection::find(FindConfig const& config)
{
    auto            response = std::make_unique<FindResult<T>>(*this, config);
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Finder<T, All>{colName(), dbName(), config, {}},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(*response, messageId, config.getParserConfig()))
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
template<typename T, typename F>
inline
FindRange<T> Collection::find(F const& search, FindConfig const& config)
{
    auto            response = std::make_unique<FindResult<T>>(mongoServer, dbName(), colName(), config);
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Finder<T, F>{colName(), dbName(), config, search},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (mongoServer.messageHandler.recvMessage(*response, messageId, config.getParserConfig()))
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

ThorsAnvil_Template_MakeOverride(2,
                        ThorsAnvil::DB::Mongo::Action::Finder,              {"minV", "min"}, {"maxV", "max"});
ThorsAnvil_Template_MakeTrait(2,
                        ThorsAnvil::DB::Mongo::Action::Finder,              find, $db, filter, sort, projection, hint, skip, limit, batchSize,
                                                                            singleBatch, comment, maxTimeMS, readConcern, maxV, minV,
                                                                            returnKey, showRecordId, tailable, awaitData, noCursorTimeout,
                                                                            allowPartialResults, collation, allowDiskUse, let);
