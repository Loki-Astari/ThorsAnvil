#ifndef THORSANVIL_SERIALIZER_PARSER_CONFIG_H
#define THORSANVIL_SERIALIZER_PARSER_CONFIG_H

#include "SerializeConfig.h"
#include "ThorsSerializerUtilTypes.h"
#include "PolymorphicMarker.h"
#include <string>
#include <utility>

namespace ThorsAnvil::Serialize
{

struct ParserConfig
{
    /*
     * These constructor are maintained for backwards compatability
     * Please use a the default constructor followed by the Set<Attribute>() methods
     */
    ParserConfig(IgnoreCallBack&& cb,
                 ParseType parseStrictness = ParseType::Weak,
                 std::string const& polymorphicMarker = Private::getDefaultPolymorphicMarker(),
                 bool catchExceptions = true)
        : parseStrictness(parseStrictness)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
        , ignoreCallBack(std::move(cb))
    {}
    ParserConfig(ParseType parseStrictness,
                 std::string const& polymorphicMarker = Private::getDefaultPolymorphicMarker(),
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
    ParserConfig& setUseOldSharedPtr()                                          {useOldSharedPtr = true;                    return *this;}
    ParserConfig& setValidateNoTrailingData()                                   {validateNoTrailingData = true;             return *this;}
    ParserConfig& setNoBackslashConversion()                                    {convertBackSlash = false;                  return *this;}
    ParseType       parseStrictness         = ParseType::Weak;
    std::string     polymorphicMarker       = Private::getDefaultPolymorphicMarker();
    bool            catchExceptions         = true;
    long            parserInfo              = 0;
    IgnoreCallBack  ignoreCallBack;
    bool            useOldSharedPtr         = false;
    bool            validateNoTrailingData  = false;
    bool            convertBackSlash        = true;
};

}

#endif
