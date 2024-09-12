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
        std::string getRawString();
        std::string getString();
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
};

class Unicode
{
    public:
        static void checkBuffer(ParserInterface& i, std::string& reply);
    private:
        static void decodeUnicode(ParserInterface& i, std::string& reply);
        static void decodeSurrogatePairs(long unicodeValue, ParserInterface& i, std::string& reply);
        static long getUnicodeHex(ParserInterface& i);
};

template<typename T>
inline T JsonManualLexer::scan()
{
    readNumber();

    char*   end;
    T value = scanValue<T>(&buffer[0], &end);
    if (buffer.size() == 0 || &buffer[0] + buffer.size() != end)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::JsonParser",
                         "scan",
                         "No data left to scan");
    }
    return value;
}

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "JsonManualLexer.source"
#endif

#endif
