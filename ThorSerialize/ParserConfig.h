#ifndef THORSANVIL_SERIALIZER_PARSER_CONFIG_H
#define THORSANVIL_SERIALIZER_PARSER_CONFIG_H

#include "SerializeConfig.h"
#include "StringInput.h"
#include "ThorsSerializerUtilTypes.h"

#include <string>
#include <variant>
#include <functional>
#include <utility>
#include <istream>

namespace ThorsAnvil::Serialize
{

using DataInputStream = std::variant<std::istream*, StringInput>;
using IdFunc = std::function<std::string(DataInputStream&)>;

struct ParserConfig
{
    /*
     * These constructor are maintained for backwards compatability
     * Please use a the default constructor followed by the Set<Attribute>() methods
     */
    ParserConfig(IgnoreCallBack&& cb,
                 ParseType parseStrictness = ParseType::Weak,
                 std::string const& polymorphicMarker = "",
                 bool catchExceptions = true)
        : parseStrictness(parseStrictness)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
        , ignoreCallBack(std::move(cb))
    {}
    ParserConfig(ParseType parseStrictness,
                 std::string const& polymorphicMarker = "",
                 bool catchExceptions = true)
        : parseStrictness(parseStrictness)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    ParserConfig(std::string const& polymorphicMarker, bool catchExceptions = true)
        : polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    ParserConfig(bool catchExceptions)
        : catchExceptions(catchExceptions)
    {}
    ParserConfig(ParseType parseStrictness, bool catchExceptions)
        : parseStrictness(parseStrictness)
        , catchExceptions(catchExceptions)
    {}
    // Use this constructor.
    ParserConfig()
    {}
    ParserConfig& setParseStrictness(ParseType p_parseStrictness)               {parseStrictness = p_parseStrictness;       return *this;}
    ParserConfig& setPolymorphicMarker(std::string const& p_polymorphicMarker)  {polymorphicMarker = p_polymorphicMarker;   return *this;}
    ParserConfig& setCatchExceptions(bool p_catchExceptions)                    {catchExceptions = p_catchExceptions;       return *this;}
    ParserConfig& setCatchUnknownExceptions(bool p_catchExceptions)             {catchUnknownExceptions = p_catchExceptions;return *this;}
    ParserConfig& setValidateNoTrailingData(bool validate = true)               {validateNoTrailingData = validate;         return *this;}
    ParserConfig& setNoBackslashConversion(bool convBS = false)                 {convertBackSlash = convBS;                 return *this;}
    ParserConfig& setIdentifyDynamicClass(IdFunc func)                          {identifyDynamcClass = std::move(func);     return *this;}
    ParserConfig& setIgnoreCallBack(IgnoreCallBack&& cb)                        {ignoreCallBack = std::move(cb);            return *this;}
    ParseType       parseStrictness         = ParseType::Weak;
    std::string     polymorphicMarker       = "";
    bool            catchExceptions         = true;
    bool            catchUnknownExceptions  = false;
    IgnoreCallBack  ignoreCallBack;
    bool            validateNoTrailingData  = false;
    bool            convertBackSlash        = true;
    IdFunc          identifyDynamcClass     = [](DataInputStream&){return "";};
};

}

#endif
