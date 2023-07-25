#ifndef THORSANVIL_DB_MONGO_THORSMONGO_KILL_CURSOR_H
#define THORSANVIL_DB_MONGO_THORSMONGO_KILL_CURSOR_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"

namespace ThorsAnvil::DB::Mongo
{

class KillCursorConfig: public ActionConfig<KillCursorConfig>
{};

struct KillCursorResult: public CmdReplyBase, public MongoActionReadInterfaceTrivialImpl<KillCursorResult>
{
    std::vector<std::uint64_t>      cursorsKilled;
    std::vector<std::uint64_t>      cursorsNotFound;
    std::vector<std::uint64_t>      cursorsAlive;
    std::vector<std::uint64_t>      cursorsUnknown;
};

}

ThorsAnvil_ExpandTrait(   ThorsAnvil::DB::Mongo::CmdReplyBase,
                          ThorsAnvil::DB::Mongo::KillCursorResult,        cursorsKilled, cursorsNotFound, cursorsAlive, cursorsUnknown);

#endif
