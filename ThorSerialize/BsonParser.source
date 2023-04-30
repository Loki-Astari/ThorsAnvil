#include "SerializeConfig.h"
#include "BsonParser.h"
#include "JsonLexemes.h"
#include "UnicodeIterator.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <map>
#include <cstdlib>
#include <cstring>

using namespace ThorsAnvil::Serialize;
using ParserToken = ParserInterface::ParserToken;


char const* getTokenTypeAsString(ParserToken result);

HEADER_ONLY_INCLUDE
BsonParser::BsonParser(std::istream& stream, ParserConfig config)
    : ParserInterface(stream, config)
    , nextToken(ParserToken::DocStart)
    , nextType('\x00')
    , skipOverValue(false)
{
    currentContainer.emplace_back(static_cast<BsonContainer>(config.parserInfo));
}

HEADER_ONLY_INCLUDE
ParserToken BsonParser::getNextToken()
{
    if (skipOverValue)
    {
        skipOverValue = false;
        if (!isEndOfContainer(0))
        {
            nextType  = '\x00';
            nextToken = ParserToken::Key;
        }
    }

    ParserToken result = nextToken;
    switch (nextToken)
    {
        case ParserToken::DocStart:
        {
            switch (currentContainer.back())
            {
                case BsonContainer::Map:    nextToken = ParserToken::MapStart;  break;
                case BsonContainer::Array:  nextToken = ParserToken::ArrayStart;break;
                case BsonContainer::Value:
                {
                    nextToken = ParserToken::Key;
                    currentContainer.emplace_back(BsonContainer::Value);
                    std::int32_t    size = readSize<4, std::int32_t>();
                    dataSize.emplace_back(size);
                    dataLeft.emplace_back(size);
                    dataLeft.back() -= 4;
                    break;
                }
            }
            break;
        }
        case ParserToken::DocEnd:
        {
            if (currentContainer.size() > 0 && currentContainer.back() == BsonContainer::Value)
            {
                readEndOfContainer();
            }
            nextToken = ParserToken::Error;
            break;
        }
        case ParserToken::MapStart:
        case ParserToken::ArrayStart:
        {
            std::int32_t    size = readSize<4, std::int32_t>();
            dataSize.emplace_back(size);
            dataLeft.emplace_back(size);
            dataLeft.back() -= 4;
            if (dataLeft.back() == 1)
            {
                nextToken = currentContainer.back() == BsonContainer::Map ? ParserToken::MapEnd: ParserToken::ArrayEnd;
                break;
            }
            nextToken = ParserToken::Key;
            break;
        }
        case ParserToken::MapEnd:
        case ParserToken::ArrayEnd:
        {
            readEndOfContainer();
            currentContainer.pop_back();
            dataLeft.pop_back();
            if (currentContainer.empty())
            {
                dataSize.pop_back();
                nextToken = ParserToken::DocEnd;
                break;
            }
            dataLeft.back() -= dataSize.back();
            dataSize.pop_back();
            if (isEndOfContainer(0))
            {
                // isEndOfContainer set the correct nextToken
                break;
            }
            nextToken = ParserToken::Key;
            break;
        }
        case ParserToken::Key:
        {
            readKey();
            nextToken = ParserToken::Value;
            BsonContainer   currentContainerType = currentContainer.back();
            if (nextType == '\x03')
            {
                currentContainer.push_back(BsonContainer::Map);
                nextToken = ParserToken::MapStart;
            }
            else if (nextType == '\x04')
            {
                currentContainer.push_back(BsonContainer::Array);
                nextToken = ParserToken::ArrayStart;
            }
            if (currentContainerType == BsonContainer::Array || currentContainerType == BsonContainer::Value)
            {
                return getNextToken();
            }
            break;
            // Array and Value fall through to read the value we
            // Want to extract next.
        }
        case ParserToken::Value:
        {
            skipOverValue = true;
            break;
        }
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "getNextToken",
                             "Invalid state reached in switch");
        }
    }
    VLOG_S(5) << getTokenTypeAsString(result);
    return result;
}

char const* getTokenTypeAsString(ParserToken result)
{
    switch (result)
    {
        case ParserToken::Error:     return "Error";
        case ParserToken::DocStart:  return "DocStart";
        case ParserToken::DocEnd:    return "DocEnd";
        case ParserToken::MapStart:  return "MapStart";
        case ParserToken::MapEnd:    return "MapEnd";
        case ParserToken::ArrayStart:return "ArrayStart";
        case ParserToken::ArrayEnd:  return "ArrayEnd";
        case ParserToken::Key:       return "Key";
        case ParserToken::Value:     return "Value";
        default:
            return "Unknown TYPE: SHOULD NOT HAPPEN";
    }
}

HEADER_ONLY_INCLUDE
void BsonParser::ignoreDataValue()
{
    if (skipOverValue == false)
    {
        return;
    }
    switch (nextType)
    {
        case '\x01':    input.ignore(8);    dataLeft.back() -= 8;   VLOG_S(5) << "Double-64";   break;
        case '\x13':    input.ignore(16);   dataLeft.back() -= 16;  VLOG_S(5) << "Double-128";  break;
        case '\x10':    input.ignore(4);    dataLeft.back() -= 4;   VLOG_S(5) << "Int-32";      break;
        case '\x12':    input.ignore(8);    dataLeft.back() -= 8;   VLOG_S(5) << "Int-64";      break;
        case '\x07':    input.ignore(12);   dataLeft.back() -= 12;  VLOG_S(5) << "Obj-ID";      break;
        case '\x08':    input.ignore(1);    dataLeft.back() -= 1;   VLOG_S(5) << "Bool";        break;
        case '\x0A':                                                VLOG_S(5) << "NULL";        break;
        case '\x02':    {std::size_t size = readSize<4, std::int32_t>();input.ignore(size);      dataLeft.back() -= (size + 4);  VLOG_S(5) << "String";break;}
        case '\x05':    {std::size_t size = readSize<4, std::int32_t>();input.ignore(size + 1);  dataLeft.back() -= (size + 5);  VLOG_S(5) << "Binary";break;}
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "ignoreDataValue",
                             "trying to ignore a non value. Type: ", static_cast<unsigned int>(static_cast<unsigned char>(nextType)));
        }
    }
}

HEADER_ONLY_INCLUDE
bool BsonParser::isEndOfContainer(std::size_t excess)
{
    VLOG_S(5) << "isEndOfContainer: dataLeft.back(): " << dataLeft.back() << " Excess: " <<  excess;
    if (dataLeft.back() - excess  == 1)
    {
        switch (currentContainer.back())
        {
            case BsonContainer::Map:    nextToken = ParserToken::MapEnd;    break;
            case BsonContainer::Array:  nextToken = ParserToken::ArrayEnd;  break;
            case BsonContainer::Value:  nextToken = ParserToken::DocEnd;    break;
        }

        return true;
    }
    return false;
}

HEADER_ONLY_INCLUDE
void BsonParser::readEndOfContainer()
{
    char    mark = '\xFF';
    if (input.read(&mark, 1))
    {
        dataLeft.back() -= 1;
        if (mark != '\x00')
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "readEndOfContainer",
                             "End of container marker should be '\\x00' but is >", static_cast<int>(mark), "<");
        }
    }
    else
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                         "readEndOfContainer",
                         "End of container marker should be '\\x00' but we failed to read any data from the stream");
    }
}

HEADER_ONLY_INCLUDE
void BsonParser::readKey()
{
    if (input.read(&nextType, 1) && std::getline(input, nextKey, '\0'))
    {
        VLOG_S(5) << "Key: " << nextKey;
        dataLeft.back() -= (1 + nextKey.size() + 1);
        return;
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "readKey",
                     "Failed to read Key");
}

HEADER_ONLY_INCLUDE
bool BsonParser::readBool()
{
    bool result;
    if (input.read(reinterpret_cast<char*>(&result) ,1))
    {
        dataLeft.back() -= 1;
        return result;
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "readBool",
                     "Failed to read Bool");
}

HEADER_ONLY_INCLUDE
std::size_t BsonParser::peekSize()
{
    std::int32_t size;

    std::streampos pos = input.tellg();
    if (input.read(reinterpret_cast<char*>(&size), 4))
    {
        input.seekg(pos);
        return boost::endian::little_to_native(size);
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "peekSize",
                     "Failed to peek at the size of the next object");
}

HEADER_ONLY_INCLUDE
std::string BsonParser::readString()
{
    std::int32_t size = readSize<4, std::int32_t>();
    dataLeft.back() -= 4;
    std::string     result(size, '\0');
    if (input.read(&result[0], size))
    {
        dataLeft.back() -= size;
        result.resize(size - 1);
        return result;
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "readString",
                     "Failed to read String");
}

HEADER_ONLY_INCLUDE
void BsonParser::readNull()
{}

HEADER_ONLY_INCLUDE
std::string BsonParser::readBinary()
{
    char            subType     = '\xFF';
    std::int32_t    size        = readSize<4, std::int32_t>();
    dataLeft.back() -= 4;

    if (input.read(reinterpret_cast<char*>(&subType), 1))
    {
        dataLeft.back() -= 1;
        std::string result(size, '\0');
        if (input.read(&result[0], size))
        {
            dataLeft.back() -= size;
            return result;
        }
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "readKBinary",
                     "Failed to read Binary Data");
}

HEADER_ONLY_INCLUDE
std::string BsonParser::getKey()
{
    return nextKey;
}

HEADER_ONLY_INCLUDE
std::string BsonParser::getRawValue()
{
    switch (nextType)
    {
        case '\x10':            VLOG_S(5) << "Double-32";return std::to_string(readInt<4, std::int32_t>());
        case '\x12':            VLOG_S(5) << "Double-64";return std::to_string(readInt<8, std::int64_t>());
        case '\x01':            VLOG_S(5) << "Double-128";return std::to_string(readFloat<8>());
#if 0
        case '\x13':            VLOG_S(5) << "Int-64";return std::to_string(readFloat<16>());
#endif
        case '\x08':            VLOG_S(5) << "Bool";return readBool() ? "true" : "false";
        case '\x0A':            VLOG_S(5) << "Null";readNull(); return "null";
        case '\x02':
        {
            using std::string_literals::operator""s;
            VLOG_S(5) << "String";
            return "\""s + readString() + "\"";
        }
        case '\x05':         VLOG_S(5) << "Binary";return readBinary();
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "getRawValue",
                             "Could not convert the data into raw output for some reason");
        }
    }
}
