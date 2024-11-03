#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H
#define THORSANVIL_DB_MONGO_THORSMONGO_H

#include "ThorsMongoConfig.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "MongoQuery.h"
#include "MongoUpdate.h"
#include "ConnectionMongo.h"
#include "ThorsMongoCommon.h"
#include "ThorsMongoInsert.h"
#include "ThorsMongoRemove.h"
#include "ThorsMongoFind.h"
#include "ThorsMongoGetMore.h"
#include "ThorsMongoKillCursor.h"
#include "ThorsMongoFindAndModify.h"
#include "ThorsMongoCount.h"
#include "ThorsMongoDistinct.h"
#include "ThorsMongoListCollection.h"
#include "ThorsMongoListDatabase.h"
#include "ThorsMongoAdmin.h"

#include "ThorSerialize/MongoUtility.h"

#include <iterator>
#include <ranges>
#include <bit>
#include <optional>

static_assert(
    std::endian::little == std::endian::native,
    "Don't want to support big endian unless I can do some good testing. Don't have a system for that so currently not suppoorted, but should be a relatively easy change"
);


namespace ThorsAnvil::DB::Mongo
{

class MongoMessageHandler: public MessageHandler
{
    ConnectionMongo mongoStream;

    public:
        MongoMessageHandler(MongoURL const& url)
            : mongoStream(url)
        {}
        MongoMessageHandler(MongoMessageHandler&& move)
            : mongoStream(std::move(move.mongoStream))
        {}
        MongoMessageHandler& operator=(MongoMessageHandler&& move)
        {
            mongoStream = std::move(move.mongoStream);
            //MessageHandler::operator=(std::move(move));
            return *this;
        }
        virtual std::iostream& getStream() override {return mongoStream;}
        ConnectionMongo&       getConnection()      {return mongoStream;}
};

class DB;
class Collection;

using OptReadConcern    = std::optional<ReadConcern>;
using OptWriteConcern   = std::optional<WriteConcern>;

enum class ThorUT { Build };
class TestAuthenticator
{
    public:
        TestAuthenticator(ThorUT const&) {}
};

class ThorsMongo
{
    struct DBInfo
    {
        OptReadConcern                      readConcern;
        OptWriteConcern                     writeConcern;
    };
    struct CollectionInfo
    {
        OptReadConcern                      readConcern;
        OptWriteConcern                     writeConcern;
    };
    MongoMessageHandler                     messageHandler;
    OptReadConcern                          readConcern;
    OptWriteConcern                         writeConcern;

    friend class DB;
    friend class Collection;
    std::map<std::string, DBInfo>           databases;
    std::map<std::string, CollectionInfo>   collections;

    public:
        ThorsMongo(MongoURL const&                  url,
                   Auth::UserNamePassword const&    authInfo,
                   Compression                      compression = Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );
        ThorsMongo(MongoURL const&                  url,
                   Auth::Certificate const&         authInfo,
                   Compression                      compression= Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );
        ThorsMongo(MongoURL const&                  url,
                   TestAuthenticator const&         authInfo,
                   Compression                      compression= Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );

        OptReadConcern const&   getReadConcern() const              {return readConcern;}
        OptReadConcern          setReadConcern(ReadConcern val)     {return std::exchange(readConcern, std::move(val));}

        OptWriteConcern const&  getWriteConcern() const             {return writeConcern;}
        OptWriteConcern         setWriteConcern(WriteConcern val)   {return std::exchange(writeConcern, std::move(val));}

        ReadPreference          getReadPreference() const;
        ReadPreference          setReadPreference(ReadPreference);

        DB operator[](std::string&& dbName);

        template<typename F>
        DBRange                 listDatabases(F const& filter, CommandConfig const& config = CommandConfig{});
        DBRange                 listDatabases(CommandConfig const& config = CommandConfig{});


    private:
    public:
        ConnectionMongo&        getStream() {return messageHandler.getConnection();}//{return mongoStream;}

    private:
        template<typename T>
        friend class CursorData;
        // These function can be used by the FindResult.
        // This is becuase the find result contains (and owns a Cursor).
        // So we need to be able to tidy that up with exposing the details to the user.
        template<typename T>
        void                    getMore(CursorData<T>& find, std::string_view dbName, std::string_view colName, std::uint64_t cursorId, GetMoreConfig const& config);
        void                    killCursor(std::string_view dbName, std::string_view colName, std::uint64_t cursorId, KillCursorConfig const& config);
};

class DB
{
    friend class Collection;
    ThorsMongo&     mongoServer;
    std::string     name;
    public:
        DB(ThorsMongo& mongoServer, std::string&& name)
            : mongoServer(mongoServer)
            , name(name)
        {}

        std::string const&      getName() const                     {return name;}

        OptReadConcern const&   getReadConcern() const              {return mongoServer.databases[name].readConcern;}
        OptReadConcern          setReadConcern(ReadConcern val)     {return std::exchange(mongoServer.databases[name].readConcern, std::move(val));}

        OptWriteConcern const&  getWriteConcern() const             {return mongoServer.databases[name].writeConcern;}
        OptWriteConcern         setWriteConcern(WriteConcern val)   {return std::exchange(mongoServer.databases[name].writeConcern, std::move(val));}

        ReadPreference          getReadPreference() const;
        ReadPreference          setReadPreference(ReadPreference);

        template<typename F>
        LCRange                 listCollections(F const& filter, CommandConfig const& config = CommandConfig{});
        LCRange                 listCollections(CommandConfig const& config = CommandConfig{});
        AdminResult             createCollection(std::string colName, CreateCollectionConfig const& config = CreateCollectionConfig{});

        Collection operator[](std::string&& collectionName);
};

class Collection
{
    ThorsMongo&     mongoServer;
    std::string     name;           // Combined Name of DB and collection so it is unique.
    public:
        Collection(DB& db, std::string&& name)
            : mongoServer(db.mongoServer)
            , name(db.name + "::" + name)
        {}

        AdminResult             rename(std::string const& name, RenameConfig const& config = RenameConfig{});
        AdminResult             drop(DropCollectionConfig const& config = DropCollectionConfig{});

        OptReadConcern const&   getReadConcern() const              {return mongoServer.collections[name].readConcern;}
        OptReadConcern          setReadConcern(ReadConcern val)     {return std::exchange(mongoServer.collections[name].readConcern, std::move(val));}

        OptWriteConcern const&  getWriteConcern() const             {return mongoServer.collections[name].writeConcern;}
        OptWriteConcern         setWriteConcern(WriteConcern val)   {return std::exchange(mongoServer.collections[name].writeConcern, std::move(val));}

        ReadPreference          getReadPreference() const;
        ReadPreference          setReadPreference(ReadPreference);

        /*
         * T => Ouptut Data Type.
         * F => Query (i.e. how to search)  See: MongoQuery.h
         * U => Update                      See: MongoUpdate.h
         */
        template<typename F>                CountResult         countDocuments(F const& query, CountConfig const& config);
                                            CountResult         countDocuments(CountConfig const& config);

        template<typename T>                InsertResult        insert(std::vector<T> const& data, InsertConfig const& config = InsertConfig{});
        template<typename... T>             InsertResult        insert(std::tuple<T...> const& data, InsertConfig const& config = InsertConfig{});

        template<typename T, typename F>    FAModifyResult<T>   findAndReplaceOne(F const& search, T const& replace, FAModifyConfig const& config = FAModifyConfig{});
        template<typename T, typename F>    FAModifyResult<T>   findAndRemoveOne(F const& search, FARemoveConfig const& config = FARemoveConfig{});
        template<typename T, typename F, typename U>
                                            FAModifyResult<T>   findAndUpdateOne(F const& search, U const& update, FAModifyConfig const& config = FAModifyConfig{});
        template<typename T, typename F, typename... U>
                                            FAModifyResult<T>   findAndUpdateOne(F const& search, std::tuple<U const&...> update, FAModifyConfig const& config = FAModifyConfig{});


        template<typename T>                RemoveResult        remove(std::vector<T> const& search, RemoveConfig const& config = RemoveConfig{});
        template<typename... T>             RemoveResult        remove(std::tuple<T...> const& search, RemoveConfig const& config = RemoveConfig{});
        template<typename T>                RemoveResult        remove(Query<T, std::string> const& search, RemoveConfig const& config = RemoveConfig{});

        template<typename T>                FindRange<T>        find(FindConfig const& config = FindConfig{});
        template<typename T, typename F>    FindRange<T>        find(F const& search, FindConfig const& config = FindConfig{});

        template<typename T, typename F>    DistinctResult<T>   distinct(std::string const& key, F const& query, DistinctConfig const& config = DistinctConfig{});
        template<typename T>                DistinctResult<T>   distinct(std::string const& key, DistinctConfig const& config = DistinctConfig{});

        std::string_view        dbName()    const {return {name.data(), name.find("::")};}
        std::string_view        colName()   const {return {name.data() + name.find("::") + 2};}

};

inline DB          ThorsMongo::operator[](std::string&& dbName)     {return DB(*this, std::move(dbName));}
inline Collection  DB::operator[](std::string&& collectionName)     {return Collection(*this, std::move(collectionName));}

// See MongoCursor.h for declaration.
template<typename T>
inline
CursorData<T>::~CursorData()
{
    if (cursor.getId() != 0) {
        owner.get().killCursor(dbName, colName, cursor.getId(), killCursorConfig);
    }
}
template<typename T>
inline
bool CursorData<T>::increment()
{
    ++index;
    if (cursor.data().size() == index)
    {
        index = 0;
        owner.get().getMore(*this, dbName, colName, cursor.getId(), getMoreConfig);
    }
    return index != cursor.data().size();
}

}

#define THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE
#include "ThorsMongoInsert.tpp"
#include "ThorsMongoRemove.tpp"
#include "ThorsMongoFind.tpp"
#include "ThorsMongoGetMore.tpp"
#include "ThorsMongoKillCursor.tpp"
#include "ThorsMongoFindAndModify.tpp"
#include "ThorsMongoCount.tpp"
#include "ThorsMongoDistinct.tpp"
#include "ThorsMongoListCollection.tpp"
#include "ThorsMongoListDatabase.tpp"
#include "ThorsMongoAdmin.tpp"
#undef THORSANVIL_DB_MONGO_THORSMONGO_H_TEMPLATE

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ThorsMongo.source"
#endif

#endif
