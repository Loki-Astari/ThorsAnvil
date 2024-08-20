#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

class KillCursors: public MongoActionWriteInterfaceTrivialImpl<KillCursors>
{
    friend class ThorsAnvil::Serialize::Traits<KillCursors>;
    std::string                     killCursors;
    std::string                     $db;
    std::vector<std::uint64_t>      cursors;

    public:
        KillCursors(std::string_view collection, std::string_view db, KillCursorConfig const& /*config*/, std::vector<std::uint64_t>&& cursors)
            : killCursors(collection)
            , $db(db)
            , cursors(std::move(cursors))
        {}
};

}


inline
void ThorsMongo::killCursor(std::string_view dbName, std::string_view colName, std::uint64_t cursorId, KillCursorConfig const& config)
{
    KillCursorResult    response;
    MessageId           messageId;
    if (messageHandler.sendMessage(Action::KillCursors{colName, dbName, config, {cursorId}},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (messageHandler.recvMessage(response, messageId, config.getParserConfig()))
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
}

}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Action::KillCursors,         killCursors, $db, cursors);
