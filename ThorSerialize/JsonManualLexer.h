#ifndef THORSANVIL_SERIALIZER_JSON_MANUAL_LEXER_H
#define THORSANVIL_SERIALIZER_JSON_MANUAL_LEXER_H

#include "SerializeConfig.h"
#include "Serialize.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <istream>

namespace ThorsAnvil::Serialize
{

class ParserInterface;
class JsonManualLexer
{
    ParserInterface&    parser;
    std::string         buffer;
    int                 lastToken;
    bool                lastBool;
    bool                lastNull;
    public:
        JsonManualLexer(ParserInterface& parser);
        int yylex();

        void        ignoreRawValue();
        std::string_view getRawString();
        std::string_view getString();
        void        getStringInto(std::string&);
        bool        getLastBool();
        bool        isLastNull();
        template<typename T>
        T scan();
    private:
        void readTrue();
        void readFalse();
        void readNull();
        void readNumber();

        void checkFixed(char const* check, std::size_t size);
        char readDigits(char next);
        void error();
        bool checkEscape(std::string& reply);
};

template<typename T>
inline T JsonManualLexer::scan()
{
    T           value;
    bool readOK = parser.readValue(value);
    if (!readOK)
    {
        ThorsLogAndThrowDebug(std::runtime_error,
                              "ThorsAnvil::Serialize::JsonManualLexer",
                              "scan",
                              "Failed to Scan a number correctly");
    }
    return value;
}

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "JsonManualLexer.source"
#endif

#endif
