#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename F>
class ListCollection: public MongoActionWriteInterfaceTrivialImpl<ListCollection<F>>
{
    // See: https://www.mongodb.com/docs/manual/reference/command/listDatabases/

    friend class ThorsAnvil::Serialize::Traits<ListCollection<F>>;

    std::uint32_t                   listCollections;
    std::string_view                $db;
    std::optional<F>                filter;
    std::optional<bool>             nameOnly;
    std::optional<bool>             authorizedCollections;
    std::optional<std::string>      comment;

    public:
        ListCollection(std::string_view db, CommandConfig const& config, std::optional<F> filter)
            : listCollections(1)
            , $db(db)
            , filter(filter)
            , nameOnly(config.getNameOnly())
            , authorizedCollections(config.getAuthorizedCollections())
            , comment(config.getComment())
        {}
};

}

template<typename F>
inline
LCRange DB::listCollections(F const& filter, CommandConfig const& config)
{
    auto            response = std::make_unique<ListCollectionResult>(mongoServer, getName(), config);
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::ListCollection<F>{getName(), config, filter},
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
    }
    return response;
}
inline
LCRange DB::listCollections(CommandConfig const& config)
{
    auto            response = std::make_unique<ListCollectionResult>(mongoServer, getName(), config);
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::ListCollection<bool>{getName(), config, {}},
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
    }
    return response;
}

}

ThorsAnvil_Template_MakeTrait(1,
                        ThorsAnvil::DB::Mongo::Action::ListCollection,  listCollections, $db, filter,
                                                                        nameOnly, authorizedCollections,
                                                                        comment);
