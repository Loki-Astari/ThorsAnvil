#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename Container>
class Remover: public MongoActionWriteInterfaceTrivialImpl<Remover<Container>>
{
    friend class ThorsAnvil::Serialize::Traits<Remover>;

    // Streamed.
    std::string_view                deleteA;        // This should be delete.
                                                    // But is a reserved word in C++
                                                    // Use: ThorsAnvil_Template_MakeOverride to fix
    std::string_view                $db;
    Cref<Container>                 deletes;
    std::optional<std::string>      comment;
    std::optional<Let>              let;
    std::optional<bool>             ordered;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::uint32_t>    maxTimeMS;
    public:
        Remover(std::string_view deleteA, std::string_view db, RemoveConfig const& config, Container const& deletes)
            : deleteA(deleteA)
            , $db(db)
            , deletes(deletes)
            , comment(config.getComment())
            , let(config.getLet())
            , ordered(config.getOrdered())
            , writeConcern(config.getWriteConcern())
            , maxTimeMS(config.getMaxTimeMS())
        {}
};


}


template<typename T>
inline
RemoveResult Collection::remove(std::vector<T> const& search, RemoveConfig const& config)
{
    RemoveResult    response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Remover<std::vector<T>>{colName(), dbName(), config, search},
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

template<typename... T>
inline
RemoveResult Collection::remove(std::tuple<T...> const& search, RemoveConfig const& config)
{
    RemoveResult    response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Remover<std::tuple<T...>>{colName(), dbName(), config, search},
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

template<typename T>
inline
RemoveResult Collection::remove(Query<T> const& search, RemoveConfig const& config)
{
    return remove(std::make_tuple(search), config);
}

}


ThorsAnvil_Template_MakeOverride(1,
                        ThorsAnvil::DB::Mongo::Action::Remover,                 {"deleteA", "delete"});
ThorsAnvil_Template_MakeTrait(1,
                        ThorsAnvil::DB::Mongo::Action::Remover,                 deleteA, $db, deletes, comment, let, ordered, writeConcern, maxTimeMS);
