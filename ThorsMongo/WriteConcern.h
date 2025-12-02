#ifndef THORSANVIL_DB_MONGO_WRITE_CONCERN_H
#define THORSANVIL_DB_MONGO_WRITE_CONCERN_H

#include "ThorsMongoConfig.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/BsonThor.h"
#include "ThorsLogging/ThorsLogging.h"
#include <optional>
#include <cstdint>
#include <string>
#include <charconv>

namespace ThorsAnvil::DB::Mongo
{

enum class W {Majority};

struct RequestAcknowledgmentSerializer;
class RequestAcknowledgment
{
    enum class WOpt {Specific, Majority};
    friend struct RequestAcknowledgmentSerializer;

    WOpt    option;
    int     count;

    public:
        RequestAcknowledgment():                    option(WOpt::Majority), count(0)    {}
        RequestAcknowledgment(W /*majority*/):      option(WOpt::Majority), count(0)    {}
        RequestAcknowledgment(int val):             option(WOpt::Specific), count(val)  {}
        RequestAcknowledgment& operator=(W)         {option = WOpt::Majority;return *this;}
        RequestAcknowledgment& operator=(int val)   {option = WOpt::Specific;count = val;return *this;}

        RequestAcknowledgment(RequestAcknowledgment const&)             = default;
        RequestAcknowledgment(RequestAcknowledgment&&)                  = default;
        RequestAcknowledgment& operator=(RequestAcknowledgment const&)  = default;
        RequestAcknowledgment& operator=(RequestAcknowledgment&&)       = default;

    friend bool operator==(RequestAcknowledgment const& lhs, RequestAcknowledgment const& rhs)  {return std::tie(lhs.option, lhs.count) == std::tie(rhs.option, rhs.count);}
    friend bool operator!=(RequestAcknowledgment const& lhs, RequestAcknowledgment const& rhs)  {return ! (lhs == rhs);}
};

struct RequestAcknowledgmentSerializer
{
    static std::size_t getPrintSizeBson(ThorsAnvil::Serialize::BsonPrinter& /*printer*/, RequestAcknowledgment const& object)
    {
        return (object.option == RequestAcknowledgment::WOpt::Specific)
            ? 4         // int 32 value.
            : 4 + 9;    // The string "majority" encoded size (4) + 9 characters.
    }
    static void writeCustom(ThorsAnvil::Serialize::PrinterInterface& printer, RequestAcknowledgment const& object)
    {
        if (object.option == RequestAcknowledgment::WOpt::Specific)
        {
            printer.addValue(object.count);
        }
        else
        {
            using namespace std::string_literals;
            printer.addValue("majority"s);
        }
    }
    static void readCustom(ThorsAnvil::Serialize::ParserInterface& parser, RequestAcknowledgment& object)
    {
        std::string_view rawValue = parser.getRawValue();
        if (rawValue.size() > 0 && rawValue[0] == '"')
        {
            using namespace std::string_literals;
            if (rawValue == "majority"s) {
                object.option = RequestAcknowledgment::WOpt::Majority;
            }
            else {
                ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::DB::Mongo::RequestAcknowledgmentSerializer", "read", "Invalid input string");
            }
        }
        else
        {
            object.option = RequestAcknowledgment::WOpt::Specific;
            char const* beg = &rawValue[0];
            char const* end = beg + rawValue.size();
            auto result = std::from_chars(beg, end, object.count);
            if (result.ptr != end) {
                throw std::invalid_argument("Invalid Number");
            }
        }
    }
};

struct WriteConcern
{
    std::optional<RequestAcknowledgment>        w;
    bool                                        j           = false;
    std::optional<std::int32_t>                 wtimeout;

    friend bool operator==(WriteConcern const& lhs, WriteConcern const& rhs) {return std::tie(lhs.w, lhs.j, lhs.wtimeout) == std::tie(rhs.w, rhs.j, rhs.wtimeout);}
    friend bool operator!=(WriteConcern const& lhs, WriteConcern const& rhs) {return !(lhs == rhs);}
};

}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::RequestAcknowledgment,   ThorsAnvil::DB::Mongo::RequestAcknowledgmentSerializer);
ThorsAnvil_MakeTrait(ThorsAnvil::DB::Mongo::WriteConcern,                           w, j, wtimeout);

#endif
