#include "SerializeConfig.h"
#include "ThorsSerializerUtil.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <string>

using std::string_literals::operator""s;

using namespace ThorsAnvil::Serialize;

std::string const ThorsAnvil::Serialize::defaultPolymorphicMarker = "__type"s;

HEADER_ONLY_INCLUDE
void ParserInterface::ignoreValue()
{
    if (config.parseStrictness != ParseType::Weak)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                         "ignoreValue",
                         "In Strict parser mode not allowed to ignore values.");
    }

    ignoreTheValue();
}

HEADER_ONLY_INCLUDE
void ParserInterface::ignoreTheMap()
{
    for (ParserToken token = getNextToken(); token != ParserToken::MapEnd; token = getNextToken())
    {
        ignoreDataValue();
        if (token != ParserToken::Key)
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheMap",
                             "Invalid token found. (Expecting Key)");
        }
        ignoreTheValue();
    }
}

HEADER_ONLY_INCLUDE
void ParserInterface::ignoreTheArray()
{
    ParserToken token = getNextToken();
    while (token != ParserToken::ArrayEnd)
    {
        switch (token)
        {
            case ParserToken::Error:
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                                 "ignoreTheArray",
                                 "Invalid token found: Error");
            }
            case ParserToken::Key:
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                                 "ignoreTheArray",
                                 "Invalid token found: Key");
            }
            case ParserToken::MapEnd:
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                                 "ignoreTheArray",
                                 "Invalid token found: MapEnd");
            }
            case ParserToken::Value:     ignoreDataValue(); break;
            case ParserToken::MapStart:  ignoreTheMap();    break;
            case ParserToken::ArrayStart:ignoreTheArray();  break;
            default:
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                                 "ignoreTheArray",
                                 "Invalid token found: Unknown");
            }
        }
        token = getNextToken();
    }
}

HEADER_ONLY_INCLUDE
void ParserInterface::ignoreTheValue()
{
    ParserToken token = getNextToken();
    switch (token)
    {
        case ParserToken::Error:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheValue",
                             "Invalid token found: Error");
        }
        case ParserToken::Key:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheValue",
                             "Invalid token found: Key");
        }
        case ParserToken::MapEnd:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheValue",
                             "Invalid token found: MapEnd");
        }
        case ParserToken::ArrayEnd:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheValue",
                             "Invalid token found: ArrayEnd");
        }
        case ParserToken::Value:     ignoreDataValue(); break;
        case ParserToken::MapStart:  ignoreTheMap();    break;
        case ParserToken::ArrayStart:ignoreTheArray();  break;
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::ParserInterface",
                             "ignoreTheValue",
                             "Invalid token found: Unknown");
        }
    }
}
