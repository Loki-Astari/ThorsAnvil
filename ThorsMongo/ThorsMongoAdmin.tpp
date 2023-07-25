#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

class RenameCollection: public MongoActionWriteInterfaceTrivialImpl<RenameCollection>
{
    friend class ThorsAnvil::Serialize::Traits<RenameCollection>;

    std::string                     renameCollection;
    std::string                     to;
    std::string                     $db = "admin";
    std::optional<bool>             dropTarget;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        RenameCollection(RenameConfig const& config, std::string_view dbName, std::string_view from, std::string_view to)
            : renameCollection(std::string(dbName) + "." + std::string(from))
            , to(std::string(dbName) + "." + std::string(to))
            , dropTarget(config.getDropTarget())
            , writeConcern(config.getWriteConcern())
            , comment(config.getComment())
        {}
};

class DropCollection: public MongoActionWriteInterfaceTrivialImpl<DropCollection>
{
    friend class ThorsAnvil::Serialize::Traits<DropCollection>;

    std::string                     drop;
    std::string                     $db;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        DropCollection(DropCollectionConfig const& config, std::string_view dbName, std::string_view colName)
            : drop(colName)
            , $db(dbName)
            , writeConcern(config.getWriteConcern())
            , comment(config.getComment())
        {}
};

class CreateCollection: public MongoActionWriteInterfaceTrivialImpl<CreateCollection>
{
    friend class ThorsAnvil::Serialize::Traits<CreateCollection>;

    std::string                     create;
    std::string                     $db;
    std::optional<bool>             capped;
    // timeSeries:                     TODO
    std::optional<std::uint32_t>    expireAfterSeconds;
    // clusteredIndex:                 TODO
    // changeStreamPreAndPostImages:   TODO
    std::optional<std::uint64_t>    size;
    std::optional<std::uint64_t>    maxSize;
    // storageEngine:                  TODO
    // validator:                      TODO
    std::optional<ValidationLevel>  validationLevel;
    // indexOptionDefaults:            TODO
    // viewOn:                         TODO
    // pipeline:                       TODO
    std::optional<Collation>        collation;
    std::optional<WriteConcern>     writeConcern;
    std::optional<std::string>      comment;

    public:
        CreateCollection(CreateCollectionConfig const& config, std::string_view dbName, std::string_view colName)
            : create(colName)
            , $db(dbName)
            , capped(config.getCapped())
            , expireAfterSeconds(config.getExpireAfterSeconds())
            , size(config.getSize())
            , maxSize(config.getMaxSize())
            , validationLevel(config.getValidationLevel())
            , collation(config.getCollation())
            , writeConcern(config.getWriteConcern())
            , comment(config.getComment())
        {}
};

}

inline
AdminResult  Collection::rename(std::string const& name, RenameConfig const& config)
{
    AdminResult     response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::RenameCollection{config, dbName(), colName(), name},
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
AdminResult  Collection::drop(DropCollectionConfig const& config)
{
    AdminResult     response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::DropCollection{config, dbName(), colName()},
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
AdminResult  DB::createCollection(std::string colName, CreateCollectionConfig const& config)
{
    AdminResult     response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::CreateCollection{config, getName(), colName},
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

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Action::RenameCollection,renameCollection, to, $db,
                                                                        dropTarget, writeConcern,
                                                                        comment);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Action::DropCollection,  drop, $db,
                                                                        writeConcern, comment);
ThorsAnvil_MakeOverride(ThorsAnvil::DB::Mongo::Action::CreateCollection,{"maxSize", "max"});
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Action::CreateCollection,create, $db, capped, expireAfterSeconds,
                                                                        size, maxSize, validationLevel,
                                                                        collation, writeConcern, comment);
