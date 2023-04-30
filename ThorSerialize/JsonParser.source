#include "SerializeConfig.h"
#include "JsonParser.h"
#include "JsonLexemes.h"
#include "UnicodeIterator.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <map>
#include <cstdlib>
#include <cstring>

// enum class ParserToken {Error, MapStart, MapEnd, ArrayStart, ArrayEnd, Key, Value};
using namespace ThorsAnvil::Serialize;
using ParserToken = ParserInterface::ParserToken;

HEADER_ONLY_INCLUDE
JsonParser::JsonParser(std::istream& stream, ParserConfig config)
    : ParserInterface(stream, config)
    , lexer(stream)
    , currentEnd(Done)
    , currentState(Init)
    , started(false)
{}

HEADER_ONLY_INCLUDE
ParserToken JsonParser::getNextToken()
{
    /* Handle States were we are not going to read any more */
    if (!started)
    {
        started = true;
        return ParserToken::DocStart;
    }
    if (currentState == Done)
    {
        currentState = Error;
        return ParserToken::DocEnd;
    }
    if (currentState == Error)
    {
        return ParserToken::Error;
    }

    // Convert Lexer tokens into smaller range 0-12
    static std::map<int, int>   tokenIndex  =
    {
        {0,                                     0},
        {'{',                                   1},
        {'}',                                   2},
        {'[',                                   3},
        {']',                                   4},
        {',',                                   5},
        {':',                                   6},
        {ThorsAnvil::Serialize::JSON_TRUE,      7},
        {ThorsAnvil::Serialize::JSON_FALSE,     8},
        {ThorsAnvil::Serialize::JSON_NULL,      9},
        {ThorsAnvil::Serialize::JSON_STRING,    10},
        //{ThorsAnvil::Serialize::JSON_INTEGER,   11},
        //{ThorsAnvil::Serialize::JSON_FLOAT,     12}
        {ThorsAnvil::Serialize::JSON_NUMBER,    13}
    };
    // State transition table;
    static State   stateTable[][14]   =
    {
        /* Token   ->   0,      1,      2,      3,      4,      5,      6,      7,      8,      9,      10,     11,     12      13*/
        /* Error */ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* Init  */ {   Error,  OpenM,  Error,  OpenA,  Error,  Error,  Error,  ValueD, ValueD, ValueD, ValueD, ValueD, ValueD, ValueD  },
        /* OpenM */ {   Error,  Error,  CloseM, Error,  Error,  Error,  Error,  Error,  Error,  Error,  Key,    Error,  Error,  Error   },
        /* Key   */ {   Error,  Error,  Error,  Error,  Error,  Error,  Colon,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* Colon */ {   Error,  OpenM,  Error,  OpenA,  Error,  Error,  Error,  ValueM, ValueM, ValueM, ValueM, ValueM, ValueM, ValueM  },
        /* ValueM*/ {   Error,  Error,  CloseM, Error,  Error,  CommaM, Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* CommaM*/ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Key,    Error,  Error,  Error   },
        /* CloseM*/ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* OpenA */ {   Error,  OpenM,  Error,  OpenA,  CloseA, Error,  Error,  ValueA, ValueA, ValueA, ValueA, ValueA, ValueA, ValueA  },
        /* ValueA*/ {   Error,  Error,  Error,  Error,  CloseA, CommaA, Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* CommaA*/ {   Error,  OpenM,  Error,  OpenA,  Error,  Error,  Error,  ValueA, ValueA, ValueA, ValueA, ValueA, ValueA, ValueA  },
        /* CloseA*/ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* ValueD*/ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
        /* Done  */ {   Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error,  Error   },
    };

    // Read the next token and update the state.
    int token   = lexer.yylex();
    int index   = tokenIndex[token];

    currentState    = stateTable[currentState][index];
    switch (currentState)
    {
        // The states that we actually want to return
        case Error:
            return ParserToken::Error;
        case Key:
            return ParserToken::Key;
        case ValueM:
            return ParserToken::Value;
        case ValueA:
            return ParserToken::Value;
        case ValueD:
            currentState = Done;
            return ParserToken::Value;
        // Punctuation.
        // Parse it but it is not the actual result
        // So try and get the next token.
        case Colon:
            return getNextToken();
        case CommaM:
            return getNextToken();
        case CommaA:
            return getNextToken();
        // We are going into a containing object.

        // Push the state we want when the containing
        // object is complete then set the state we will
        // need if we open another container.
        case OpenM:
            parrentState.push_back(currentEnd);
            currentEnd  = ValueM;
            return ParserToken::MapStart;
        case OpenA:
            parrentState.push_back(currentEnd);
            currentEnd  = ValueA;
            return ParserToken::ArrayStart;

        // We are leaving the containing object.
        // Pop the state we previously saved.
        case CloseM:
            currentEnd  = currentState    = parrentState.back();
            parrentState.pop_back();
            return ParserToken::MapEnd;
        case CloseA:
            currentEnd  = currentState    = parrentState.back();
            parrentState.pop_back();
            return ParserToken::ArrayEnd;

        // Anything else is an error.
        // We don't have a default as it should never happen.
        // If it does happen we fall through anyway to the throw statement below.
        default: break;
    }
    // If we hit anything else there was a serious problem in the
    // parser itself.
    ThorsLogAndThrow("ThorsAnvil::Serialize::JsonParser",
                     "getNextToken",
                     "Reached an Unnamed State");
}

HEADER_ONLY_INCLUDE
std::string JsonParser::getString()
{
    return lexer.getString();
}
HEADER_ONLY_INCLUDE
std::string JsonParser::getRawString()
{
    return lexer.getRawString();
}
HEADER_ONLY_INCLUDE
void JsonParser::ignoreDataValue()
{
    lexer.ignoreRawValue();
}

HEADER_ONLY_INCLUDE
std::string JsonParser::getKey()
{
    return getString();
}

template<typename T>
inline T JsonParser::scan()
{
    return lexer.scan<T>();
}

HEADER_ONLY_INCLUDE void JsonParser::getValue(short int& value)                     {value = scan<short int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(int& value)                           {value = scan<int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(long int& value)                      {value = scan<long int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(long long int& value)                 {value = scan<long long int>();}

HEADER_ONLY_INCLUDE void JsonParser::getValue(unsigned short int& value)            {value = scan<unsigned short int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(unsigned int& value)                  {value = scan<unsigned int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(unsigned long int& value)             {value = scan<unsigned long int>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(unsigned long long int& value)        {value = scan<unsigned long long int>();}

HEADER_ONLY_INCLUDE void JsonParser::getValue(float& value)                         {value = scan<float>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(double& value)                        {value = scan<double>();}
HEADER_ONLY_INCLUDE void JsonParser::getValue(long double& value)                   {value = scan<long double>();}

HEADER_ONLY_INCLUDE void JsonParser::getValue(bool& value)
{
    value = lexer.getLastBool();
}

HEADER_ONLY_INCLUDE
void JsonParser::getValue(std::string& value)
{
    value = getString();
}

HEADER_ONLY_INCLUDE
bool JsonParser::isValueNull()
{
    return lexer.isLastNull();
}

HEADER_ONLY_INCLUDE
std::string JsonParser::getRawValue()
{
    return getRawString();
}
