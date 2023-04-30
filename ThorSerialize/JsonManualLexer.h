#ifndef THORS_ANVIL_SERIALIZER_JSON_MANUAL_LEXER_H
#define THORS_ANVIL_SERIALIZER_JSON_MANUAL_LEXER_H

#include "Serialize.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <istream>

namespace ThorsAnvil
{
    namespace Serialize
    {

class JsonManualLexer
{
    std::istream&       str;
    std::string         buffer;
    int                 lastToken;
    bool                lastBool;
    bool                lastNull;
    public:
        JsonManualLexer(std::istream& str);
        int yylex();

        void        ignoreRawValue();
        std::string getRawString();
        std::string getString();
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
}

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "JsonManualLexer.source"
#endif

#endif
