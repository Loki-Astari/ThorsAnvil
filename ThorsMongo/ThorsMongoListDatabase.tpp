#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename F>
class ListDatabase: public MongoActionWriteInterfaceTrivialImpl<ListDatabase<F>>
{
    friend class ThorsAnvil::Serialize::Traits<ListDatabase<F>>;

    std::uint32_t                   listDatabases;
    std::string                     $db = "admin";
    std::optional<F>                filter;
    std::optional<bool>             nameOnly;
    std::optional<bool>             authorizedCollections;
    std::optional<std::string>      comment;

    public:
        ListDatabase(CommandConfig const& config, std::optional<F> filter)
            : listDatabases(1)
            , filter(filter)
            , nameOnly(config.getNameOnly())
            , authorizedCollections(config.getAuthorizedCollections())
            , comment(config.getComment())
        {}
};

}

template<typename F>
inline
DBRange ThorsMongo::listDatabases(F const& filter, CommandConfig const& config)
{
    auto            response = std::make_unique<ListDatabaseResult>();
    MessageId       messageId;
    if (messageHandler.sendMessage(Action::ListDatabase<F>{config, filter},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (messageHandler.recvMessage(*response, messageId, config.getParserConfig()))
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
inline
DBRange ThorsMongo::listDatabases(CommandConfig const& config)
{
    auto            response = std::make_unique<ListDatabaseResult>();
    MessageId       messageId;
    if (messageHandler.sendMessage(Action::ListDatabase<bool>{config, {}},
                                               messageId,
                                               config.getMsgFlag(),
                                               config.getPrinterConfig()))
    {
        if (messageHandler.recvMessage(*response, messageId, config.getParserConfig()))
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
                        ThorsAnvil::DB::Mongo::Action::ListDatabase,    listDatabases, $db, filter,
                                                                        nameOnly, authorizedCollections,
                                                                        comment);
