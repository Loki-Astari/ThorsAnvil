#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#error 'This should not be included directly: Include "ThorsMongo.h"'
#endif

#include <string_view>

namespace ThorsAnvil::DB::Mongo
{

namespace Action
{

template<typename T>
struct HasID
{
    using One = char;
    struct Two { char x[2]; };


    template <typename C> static One testHasID( decltype(C::_id)* );
    template <typename C> static Two testHasID(...);

    static constexpr bool val = sizeof(testHasID<T>(nullptr)) == sizeof(char);
};

template<typename Container>
struct FirstType;

template<typename T>
struct FirstType<std::vector<T>>
{
    using ValueType = T;
};

template<typename... T>
struct FirstType<std::tuple<T...>>
{
    using ValueType = typename std::tuple_element<0, std::tuple<T...>>::type;
};

template<typename T>
using FirstType_T = typename FirstType<T>::ValueType;

template<typename Container>
class Inserter: public MongoActionWriteInterface
{
    // Streamed.
    std::string_view                insert;
    std::string_view                $db;
    Cref<Container>                 documents;
    std::optional<bool>             ordered;
    std::optional<std::uint32_t>    maxTimeMS;
    std::optional<WriteConcern>     writeConcern;
    std::optional<bool>             bypassDocumentValidation;
    std::optional<std::string>      comment;

    // Not-Streamed
    InsertResult&           result;

    friend class ThorsAnvil::Serialize::Traits<Inserter<Container>>;
    public:
        Inserter(std::string_view insert, std::string_view db, InsertConfig const& config, Container const& documents, InsertResult& result)
            : insert(insert)
            , $db(db)
            , documents(documents)
            , ordered(config.getOrdered())
            , maxTimeMS(config.getMaxTimeMS())
            , writeConcern(config.getWriteConcern())
            , bypassDocumentValidation(config.getBypassDocumentValidation())
            , comment(config.getComment())
            , result(result)
        {}
        virtual std::uint32_t getSizeBson() const override
        {
            if constexpr (HasID<FirstType_T<Container>>::val) {
                return ThorsAnvil::Serialize::bsonGetPrintSize(*this);
            }
            else
            {
                // Need to modify the config.
                // So we get the expanded size to handle the _id field we are inserting.
                return ThorsAnvil::Serialize::bsonGetPrintSize(*this, {{}, result.inserted});
            }
        }
        virtual void writeBson(std::ostream& stream, PrinterConfig const& config) const override
        {
            if constexpr (HasID<FirstType_T<Container>>::val)
            {
                // The ID are already available.
                // Copy them to the output.
                for (auto const& v: documents.get()) {
                    result.inserted.emplace_back(v._id);
                }
                stream << ThorsAnvil::Serialize::bsonExporter(*this, config);
            }
            else
            {
                // There are no ID in the objet.
                // Ask the serializer to generate and add to the outgoing object.
                stream << ThorsAnvil::Serialize::bsonExporter(*this, {config, result.inserted});
            }
        }
};

}


template<typename T>
inline
InsertResult Collection::insert(std::vector<T> const& data, InsertConfig const& config)
{
    InsertResult    response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Inserter<std::vector<T>>{colName(), dbName(), config, data, response},
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
InsertResult Collection::insert(std::tuple<T...> const& data, InsertConfig const& config)
{
    InsertResult    response;
    MessageId       messageId;
    if (mongoServer.messageHandler.sendMessage(Action::Inserter<std::tuple<T...>>{colName(), dbName(), config, data, response},
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
                            ThorsAnvil::DB::Mongo::Action::Inserter,            insert, $db, documents, ordered, maxTimeMS, writeConcern, bypassDocumentValidation, comment);
