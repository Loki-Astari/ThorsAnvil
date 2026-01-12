#ifndef THORSANVIL_DB_MONGO_THORSMONGO_LIST_DATABASE_H
#define THORSANVIL_DB_MONGO_THORSMONGO_LIST_DATABASE_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include "ThorsMongoCommandConfig.h"
#include "MongoCursor.h"


namespace ThorsAnvil::DB::Mongo
{

struct DBInfo
{
    std::string         name;
    std::uint64_t       sizeOnDisk;
    bool                empty;
};

struct ListDatabaseResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<ListDatabaseResult>
{
    using ValueType = DBInfo;

    std::vector<DBInfo> databases;
    std::uint64_t       totalSize   = 0;
    std::uint64_t       totalSizeMb = 0;


    // Index and following three functions are used by Range.
    private:
    public:
        bool checkAvailable(std::size_t index)  {return index != databases.size();}
        DBInfo& get(std::size_t index)          {return databases[index];}
        std::size_t size() const                {return databases.size();}
};

using DBRange = Range<ListDatabaseResult>;

}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::DBInfo,                  name, sizeOnDisk, empty);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::ListDatabaseResult,      databases, totalSize, totalSizeMb);


#endif
