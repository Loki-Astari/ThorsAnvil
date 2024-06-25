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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
BsonParser::BsonParser(std::istream& stream, ParserConfig config)
    : ParserInterface(stream, config)
    , nextToken(ParserToken::DocStart)
    , nextType('\x00')
    , skipOverValue(false)
{
    currentContainer.emplace_back(static_cast<BsonContainer>(config.parserInfo));
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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
    ThorsMessage(5, "BsonParser", "getNextToken", getTokenTypeAsString(result));
    return result;
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
void BsonParser::ignoreDataMap(bool begin)
{
    if (begin)
    {
        std::int32_t    size = 0;
        config.ignoreCallBack.append("\x03", 1);
        config.ignoreCallBack.append(nextKey.c_str(), nextKey.size() + 1);
        config.ignoreCallBack.append(reinterpret_cast<char*>(&size), sizeof(size));
    }
    else
    {
        config.ignoreCallBack.append("\0x00", 1);
    }
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
void BsonParser::ignoreDataArray(bool begin)
{
    if (begin)
    {
        std::int32_t    size = 0;
        config.ignoreCallBack.append("\x04", 1);
        config.ignoreCallBack.append(nextKey.c_str(), nextKey.size() + 1);
        config.ignoreCallBack.append(reinterpret_cast<char*>(&size), sizeof(size));
    }
    else
    {
        config.ignoreCallBack.append("\0x00", 1);
    }
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
void BsonParser::ignoreDataValue()
{
    if (skipOverValue == false)
    {
        return;
    }
    using AppendFunc = typename IgnoreCallBack::AppendFunc;
    using ReadFunc   = typename IgnoreCallBack::ReadFunc;
    using IgnoreFunc = typename IgnoreCallBack::IgnoreFunc;
    AppendFunc& append  = config.ignoreCallBack.append;
    ReadFunc&   read    = config.ignoreCallBack.read;
    IgnoreFunc& ignore  = config.ignoreCallBack.ignore;

    append(&nextType, 1);
    append(nextKey.c_str(), nextKey.size() + 1);

    switch (nextType)
    {
        /*
            Notes:  0x06: => Deprecated
                    0x03: => Map
                    0x04: => Array
                    0x0B: => RegExpression  Not supported (don't understand use case)
                    0x0C: => Deprecated
                    0x0D: => Javascript     Not supported (don't understand use case)
                    0x0E: => Deprecated
                    0x0F: => Deprecated
                    -1:   => Min Key        Not supported (don't understand use case)
                    127:  => Max Key        Not supported (don't understand use case)
        */
        case '\x01':    ignore(input, 8);   dataLeft.back() -= 8;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "Double-64");   break;
        case '\x13':    ignore(input, 16);  dataLeft.back() -= 16;  ThorsMessage(5, "BsonParser", "ignoreDataValue", "Double-128");  break;
        case '\x10':    ignore(input, 4);   dataLeft.back() -= 4;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "Int-32");      break;
        case '\x12':    ignore(input, 8);   dataLeft.back() -= 8;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "Int-64");      break;
        case '\x07':    ignore(input, 12);  dataLeft.back() -= 12;  ThorsMessage(5, "BsonParser", "ignoreDataValue", "Obj-ID");      break;
        case '\x08':    ignore(input, 1);   dataLeft.back() -= 1;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "Bool");        break;
        case '\x09':    ignore(input, 8);   dataLeft.back() -= 8;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "UTC DateTime");break;
        case '\x11':    ignore(input, 8);   dataLeft.back() -= 8;   ThorsMessage(5, "BsonParser", "ignoreDataValue", "TimeStamp");   break;
        case '\x0A':                                                ThorsMessage(5, "BsonParser", "ignoreDataValue", "NULL");        break;
        case '\x02':
        {
            std::int32_t size;
            read(input, reinterpret_cast<char*>(&size), sizeof(size));
            boost::endian::little_to_native(size);
            ignore(input, size);
            dataLeft.back() -= (size + 4);
            ThorsMessage(5, "BsonParser", "ignoreDataValue", "String");
            break;
        }
        case '\x05':
        {
            std::int32_t size;
            read(input, reinterpret_cast<char*>(&size), sizeof(size));
            boost::endian::little_to_native(size);
            ignore(input, size + 1);
            dataLeft.back() -= (size + 5);
            ThorsMessage(5, "BsonParser", "ignoreDataValue", "Binary");
            break;
        }
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "ignoreDataValue",
                             "trying to ignore a non value. Type: ", static_cast<unsigned int>(static_cast<unsigned char>(nextType)));
        }
    }
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
bool BsonParser::isEndOfContainer(std::size_t excess)
{
    ThorsMessage(5, "BsonParser", "isEndOfContainer", "isEndOfContainer: dataLeft.back(): ", dataLeft.back(), " Excess: ",  excess);
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
void BsonParser::readKey()
{
    if (input.read(&nextType, 1) && std::getline(input, nextKey, '\0'))
    {
        ThorsMessage(5, "BsonParser", "readKey", "Key: ", nextKey);
        dataLeft.back() -= (1 + nextKey.size() + 1);
        return;
    }
    ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                     "readKey",
                     "Failed to read Key");
}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
void BsonParser::readNull()
{}

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
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

THORS_SERIALIZER_HEADER_ONLY_INCLUDE
std::string BsonParser::getKey()
{
    return nextKey;
}


/*
 * Note: This functionality is used by depricated methods.
 *       It only supports conversion from binary to string and then being parsed into a value.
 *       See:           ThorsAnvil_MakeTraitCustom for details.
 *       Prefer to use: ThorsAnvil_SelfSerialize   for a better experience.
 */
THORS_SERIALIZER_HEADER_ONLY_INCLUDE
std::string BsonParser::getRawValue()
{
    switch (nextType)
    {
        case '\x01':            ThorsMessage(5, "BsonParser", "getRawValue", "Double-64");return std::to_string(readFloat<8>());
        case '\x10':            ThorsMessage(5, "BsonParser", "getRawValue", "Int-32");return std::to_string(readInt<4, std::int32_t>());
        case '\x12':            ThorsMessage(5, "BsonParser", "getRawValue", "Int-64");return std::to_string(readInt<8, std::int64_t>());
        case '\x08':            ThorsMessage(5, "BsonParser", "getRawValue", "Bool");return readBool() ? "true" : "false";
        case '\x0A':            ThorsMessage(5, "BsonParser", "getRawValue", "Null");readNull(); return "null";
        case '\x02':
        {
            using std::string_literals::operator""s;
            ThorsMessage(5, "BsonParser", "getRawValue", "String");
            return "\""s + readString() + "\"";
        }
        case '\x05':         ThorsMessage(5, "BsonParser", "getRawValue", "Binary");return readBinary();
        default:
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonParser",
                             "getRawValue",
                             "Could not convert the data into raw output for some reason");
        }
    }
}
