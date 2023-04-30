#include "SerializeConfig.h"
#include "JsonManualLexer.h"
#include "JsonLexemes.h"
#include "UnicodeIterator.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"

#include <limits>
#include <cstring>

using namespace ThorsAnvil::Serialize;

HEADER_ONLY_INCLUDE
JsonManualLexer::JsonManualLexer(std::istream& str)
    : str(str)
    , lastNull(false)
{}

HEADER_ONLY_INCLUDE
int JsonManualLexer::yylex()
{
    char    next;
    str >> next;
    buffer.clear();
    lastNull = false;
    switch (next)
    {
        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
        case ':':
        {
            return lastToken = next;
        }
        case 't':
        {
            str.unget();
            lastBool = true;
            return lastToken = ThorsAnvil::Serialize::JSON_TRUE;
        }
        case 'f':
        {
            str.unget();
            lastBool = false;
            return lastToken = ThorsAnvil::Serialize::JSON_FALSE;
        }
        case 'n':
        {
            str.unget();
            lastNull = true;
            return lastToken = ThorsAnvil::Serialize::JSON_NULL;
        }
        case '"':
        {
            str.unget();
            return lastToken = ThorsAnvil::Serialize::JSON_STRING;
        }
        default:
        {
            str.unget();
            return lastToken = ThorsAnvil::Serialize::JSON_NUMBER;
        }
    }
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::readTrue()
{
    checkFixed("true", 4);
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::readFalse()
{
    checkFixed("false", 5);
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::readNull()
{
    checkFixed("null", 4);
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::ignoreRawValue()
{
    switch (lastToken)
    {
        case ThorsAnvil::Serialize::JSON_TRUE:      str.ignore(4);break;
        case ThorsAnvil::Serialize::JSON_FALSE:     str.ignore(5);break;
        case ThorsAnvil::Serialize::JSON_NULL:      str.ignore(4);break;
        case ThorsAnvil::Serialize::JSON_NUMBER:    readNumber();break;
        case ThorsAnvil::Serialize::JSON_STRING:
        {
            char last = str.get();  // Read the first Quote off the stream
            int next = str.get();
            while (next != EOF && !(next == '"' && last != '\\'))
            {
                last = next;
                next = str.get();
            }
            if (next == EOF)
            {
                error();
            }
            break;
        }
        default:break;
    }
}

HEADER_ONLY_INCLUDE
std::string JsonManualLexer::getRawString()
{
    switch (lastToken)
    {
        case '{':   return "{";
        case '}':   return "}";
        case '[':   return "[";
        case ']':   return "]";
        case ',':   return ",";
        case ':':   return ":";
        case ThorsAnvil::Serialize::JSON_TRUE:  str.ignore(4);return "true";
        case ThorsAnvil::Serialize::JSON_FALSE: str.ignore(5);return "false";
        case ThorsAnvil::Serialize::JSON_NULL:  str.ignore(4);return "null";
        //case ThorsAnvil::Serialize::JSON_INTEGER:
        //case ThorsAnvil::Serialize::JSON_FLOAT:
        case ThorsAnvil::Serialize::JSON_NUMBER:
        {
            readNumber();
            return buffer;
        }
        case ThorsAnvil::Serialize::JSON_STRING:
        {
            std::string result;

            char last = str.get();  // Read the first Quote off the stream
            result.push_back(last);
            int next = str.get();
            while (next != EOF && !(next == '"' && last != '\\'))
            {
                if (next < 0x20)
                {
                    ThorsLogAndThrow("ThorsAnvil::Serializer::JsonManualLexer",
                                     "getRawString",
                                     "Strings should not contain control characters.");
                }
                result.push_back(next);
                last = next;
                next = str.get();
            }
            if (next == EOF)
            {
                error();
            }
            result.push_back('"');
            return result;
        }
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serializer::JsonManualLexer",
                             "getRawString",
                             "Don't know how to retrieve the RAW data from the JSON input");
        }
    }
}

HEADER_ONLY_INCLUDE
std::string JsonManualLexer::getString()
{
    return std::string(make_UnicodeWrapperIterator(std::istreambuf_iterator<char>(str)), make_EndUnicodeWrapperIterator(std::istreambuf_iterator<char>(str)));
}

HEADER_ONLY_INCLUDE
bool JsonManualLexer::getLastBool()
{
    switch (lastToken)
    {
        case ThorsAnvil::Serialize::JSON_TRUE:
            readTrue();
            return lastBool;
        case ThorsAnvil::Serialize::JSON_FALSE:
            readFalse();
            return lastBool;
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::JsonParser",
                             "getLastBool",
                             "The last value was not a bool");
        }
    }
}

HEADER_ONLY_INCLUDE
bool JsonManualLexer::isLastNull()
{
    return lastNull;
}

HEADER_ONLY_INCLUDE
char JsonManualLexer::readDigits(char next)
{
    if (!std::isdigit(next))
    {
        error();
    }
    while (std::isdigit(next))
    {
        buffer.push_back(next);
        next = str.get();
    }
    return next;
}
HEADER_ONLY_INCLUDE
void JsonManualLexer::readNumber()
{
    buffer.clear();

    int next = str.get();

    if (next == '-' || next == '+')
    {
        buffer.push_back(next);
        next = str.get();
        if (next == EOF)
        {   error();
        }
    }
    if (next == '0')
    {
        buffer.push_back(next);
        next = str.get();
    }
    else
    {
        next = readDigits(next);
    }
    if (next == '.')
    {
        buffer.push_back(next);
        next        = str.get();
        if (next == EOF)
        {   error();
        }
        next        = readDigits(next);
    }
    if (next == 'e' || next == 'E')
    {
        buffer.push_back(next);
        next = str.get();
        if (next == EOF)
        {   error();
        }
        if (next == '-' || next == '+')
        {
            buffer.push_back(next);
            next = str.get();
            if (next == EOF)
            {   error();
            }
        }
        next = readDigits(next);
    }
    if (next != EOF)
    {
        str.unget();
    }
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::checkFixed(char const* check, std::size_t size)
{
    buffer.resize(size);
    str.read(&buffer[0], size);
    if (std::strncmp(&buffer[0], check, size) != 0)
    {
        error();
    }
}

HEADER_ONLY_INCLUDE
void JsonManualLexer::error()
{
    ThorsLogAndThrow("ThorsAnvil::Serialize::JsonManualLexer",
                     "error",
                     "Invalid Character in Lexer");
}
