#ifndef THORSANVIL_DB_MONGO_READ_CONCERN_H
#define THORSANVIL_DB_MONGO_READ_CONCERN_H

#include "ThorsMongoConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/BsonPrinter.h"

namespace ThorsAnvil::DB::Mongo
{

enum class Level {Local, Available, Majority, Linearizable, Snapshot};
using namespace std::literals;

struct LevelSerializer
{
    static constexpr std::string_view kLocal        = "local"sv;
    static constexpr std::string_view kAvailable    = "available"sv;
    static constexpr std::string_view kMajority     = "majority"sv;
    static constexpr std::string_view kLinearizable = "linearizable"sv;
    static constexpr std::string_view kSnapshot     = "snapshot"sv;

    static std::size_t getPrintSizeBson(ThorsAnvil::Serialize::BsonPrinter& /*printer*/, Level const& object)
    {
        switch (object)
        {
            case Level::Local:          return 4 + 6;
            case Level::Available:      return 4 + 10;
            case Level::Majority:       return 4 + 9;
            case Level::Linearizable:   return 4 + 13;
            case Level::Snapshot:       return 4 + 9;
        }
        return 0;
    }
    static void writeCustom(ThorsAnvil::Serialize::PrinterInterface& printer, Level const& object)
    {
        using namespace std::string_literals;
        switch (object)
        {
            case Level::Local:          printer.addValue(kLocal);       break;
            case Level::Available:      printer.addValue(kAvailable);   break;
            case Level::Majority:       printer.addValue(kMajority);    break;
            case Level::Linearizable:   printer.addValue(kLinearizable);break;
            case Level::Snapshot:       printer.addValue(kSnapshot);    break;
        }
    }
    static void readCustom(ThorsAnvil::Serialize::ParserInterface& parser, Level& object)
    {
        std::string value;
        parser.getValue(value);

        if (value == kLocal)              {object = Level::Local;}
        else if (value == kAvailable)     {object = Level::Available;}
        else if (value == kMajority)      {object = Level::Majority;}
        else if (value == kLinearizable)  {object = Level::Linearizable;}
        else if (value == kSnapshot)      {object = Level::Snapshot;}
    }
};

struct ReadConcern
{
    Level       level   = Level::Local;

    friend bool operator==(ReadConcern const& lhs, ReadConcern const& rhs) {return lhs.level == rhs.level;}
    friend bool operator!=(ReadConcern const& lhs, ReadConcern const& rhs) {return !(lhs == rhs);}
};

}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::Level,   ThorsAnvil::DB::Mongo::LevelSerializer);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::ReadConcern,            level);

#endif
