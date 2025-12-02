#ifndef THORSANVIL_SERIALIZE_BSON_PARSER_H
#define THORSANVIL_SERIALIZE_BSON_PARSER_H

/*
 * BsonParser<T>
 *      This is used in conjunction with BsonPrinter<T>
 *
 *      Together these provide an implementation of:
 *          the ParserInterface for type T
 *          and PrinterInterface for type T
 *
 *      These Interfaces are used by Serializer and DeSerializer (see Serialize.h)
 *
 *      It uses ThorsAnvil::Serialize::Traits<T> to know what objects to pull from the stream.
 *      For arrays order is important.
 *      For Objects the order of elements is not important. It looks up the key in the Traits<T>
 *      information to understand which member is being de-serialized but unspecified elements
 *      will not cause an error.
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include "BsonUtil.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <istream>
#include <string>
#include <vector>
#include <bit>

static_assert(
    std::endian::little == std::endian::native,
    "Don't want to support big endian unless I can do some good testing. Don't have a system for that so currently not suppoorted, but should be a relatively easy change"
);


namespace ThorsAnvil::Serialize
{

class BsonParser;
namespace MongoUtility
{
    class UTCDateTime;
    BsonParser& operator>>(BsonParser& parser, MongoUtility::UTCDateTime& data);
}

class BsonParser: public ParserInterface
{
    friend BsonParser& MongoUtility::operator>>(BsonParser& parser, MongoUtility::UTCDateTime& data);

    std::vector<BsonContainer>  currentContainer;
    std::vector<std::size_t>    dataLeft;
    std::vector<std::size_t>    dataSize;
    ParserToken                 nextToken;
    std::string                 nextKey;
    char                        nextType;
    bool                        skipOverValue;


    public:
        BsonParser(std::istream& stream, ParserConfig config = ParserConfig{});
        BsonParser(std::string_view const& stream, ParserConfig config = ParserConfig{});
        virtual FormatType formatType()                         override {return FormatType::Bson;}
        virtual ParserToken getNextToken()                      override;
        virtual std::string_view getKey()                       override;

        virtual void    ignoreDataValue()                       override;
        virtual void    ignoreDataMap(bool)                     override;
        virtual void    ignoreDataArray(bool)                   override;

        virtual void    getValue(short int& value)              override    {value = static_cast<int>(getIntValue<MaxTemplate<4, sizeof(short int)>::value, short int>());}
        virtual void    getValue(int& value)                    override    {value = getIntValue<sizeof(int), int>();}
        virtual void    getValue(long int& value)               override    {value = getIntValue<sizeof(long int), long int>();}
        virtual void    getValue(long long int& value)          override    {value = getIntValue<sizeof(long long int), long long int>();}

        virtual void    getValue(unsigned short int& value)     override    {value = getIntValue<MaxTemplate<4, sizeof(unsigned short int)>::value, unsigned short int>();}
        virtual void    getValue(unsigned int& value)           override    {value = getIntValue<sizeof(unsigned int), unsigned int>();}
        virtual void    getValue(unsigned long int& value)      override    {value = getIntValue<sizeof(unsigned long int), unsigned long int>();}
        virtual void    getValue(unsigned long long int& value) override    {value = getIntValue<sizeof(unsigned long long int), unsigned long long int>();}

        virtual void    getValue(float& value)                  override    {value = getFloatValue<8, float>();}
        virtual void    getValue(double& value)                 override    {value = getFloatValue<8, double>();}
        virtual void    getValue(long double& value)            override    {value = getFloatValue<8, long double>();}

        virtual void    getValue(bool& value)                   override    {if (nextType != '\x08')    {badType("Bool", nextType);}value = readBool();}

        virtual void    getValue(std::string& value)            override    {if (nextType != '\x02')    {badType("String", nextType);}value.clear();readString(value);}

        virtual bool    isValueNull()                           override    {return (nextType == '\x0A');}

        virtual std::string_view getRawValue()                  override;

        void useStreamData(std::size_t amount) {dataLeft.back() -= amount;}

    public:
        char getValueType() const     {return nextType;}
        template<std::size_t size, typename Int> Int readLE()
        {
            Int docSize;
            read(reinterpret_cast<char*>(&docSize), size);
            return docSize;
        }

        template<std::size_t size, typename Int> Int readBE()
        {
            Int     docSize = 0;
            char*   docData = reinterpret_cast<char*>(&docSize);
            read(docData + sizeof(docSize) - size, size);
            for (std::size_t loop = 0; loop < sizeof(docSize)/2; ++loop) {
                std::swap(docData[loop], docData[sizeof(docSize) - loop - 1]);
            }
            return docSize;
        }


    private:
        template<std::size_t size, typename Int>
        Int readSize();
        bool isEndOfContainer(std::size_t unread);
        //std::size_t peekSize();

        void readEndOfContainer();

        template<std::size_t Size, typename Int>
        Int getIntValue();
        template<std::size_t Size, typename Float>
        Float getFloatValue();

        void readKey();

        template<std::size_t Size, typename Int>
        Int readInt();

        template<typename F>
        F readFloat();

        bool readBool();
        void readString(std::string& buffer);
        void readNull();
        void readBinary(std::string& buffer);

        [[noreturn]]
        void badType(std::string const& expected, unsigned char got)
        {
            std::string gotName;
            switch (got)
            {
                case 0x01: gotName = "double";                     break;
                case 0x02: gotName = "string";                     break;
                case 0x03: gotName = "map";                        break;
                case 0x04: gotName = "array";                      break;
                case 0x05: gotName = "binary";                     break;
                case 0x06: gotName = "Value - Deprecated";         break;
                case 0x07: gotName = "ObjectId";                   break;
                case 0x08: gotName = "Boolean";                    break;
                case 0x09: gotName = "UTC datetime";               break;
                case 0x0A: gotName = "Null";                       break;
                case 0x0B: gotName = "Regular expression";         break;
                case 0x0C: gotName = "DBPointer — Deprecated";     break;
                case 0x0D: gotName = "JavaScript code";            break;
                case 0x0E: gotName = "Symbol";                     break;
                case 0x0F: gotName = "JavaScript Deprecated";      break;
                case 0x10: gotName = "int32";                      break;
                case 0x11: gotName = "Timestamp";                  break;
                case 0x12: gotName = "int64";                      break;
                case 0x13: gotName = "decimal128";                 break;
                case 0xFF: gotName = "Min key";                    break;
                case 0x7F: gotName = "Max key";                    break;
                default:
                    gotName = "Unknown";
            }
            ThorsLogAndThrowDebug(std::runtime_error,
                                  "ThorsAnvil::Serialize::BsonParser",
                                  "badType",
                                  "Trying to read a type that we can can't convert.",
                                  "Expected: ", expected, " Got: ", gotName, " : ", got);
        }
};

template<std::size_t size, typename Int>
inline Int BsonParser::readSize()
{
    return readLE<size, Int>();
}

template<std::size_t size, typename Int>
inline Int BsonParser::readInt()
{
    dataLeft.back() -= size;
    return readLE<size, Int>();
}

template<typename F>
inline F BsonParser::readFloat()
{
    F result;
    if (read(reinterpret_cast<char*>(&result), sizeof(F)))
    {
        dataLeft.back() -= sizeof(F);
        return result;
    }
    ThorsLogAndThrowDebug(std::runtime_error,
                          "ThorsAnvil::Serialize::BsonParser",
                          "readFloat",
                          "Failed to read Float Value. Size: ", sizeof(F));
}

template<std::size_t Size, typename Int>
inline Int BsonParser::getIntValue()
{
    if (nextType == '\x10')     {ThorsMessage(5, "BsonParser", "getIntValue", "Int-32"); return static_cast<Int>(readInt<4, std::int32_t>());}
    if (nextType == '\x12')     {ThorsMessage(5, "BsonParser", "getIntValue", "Int-64"); return static_cast<Int>(readInt<8, std::int64_t>());}
    badType("Int(32 or 64)", nextType);
}

template<std::size_t Size, typename Float>
inline Float BsonParser::getFloatValue()
{
    if (nextType == '\x10')     {ThorsMessage(5, "BsonParser", "getFloatValue", "Double-32");return static_cast<Float>(readInt<4, std::int32_t>());}
    if (nextType == '\x12')     {ThorsMessage(5, "BsonParser", "getFloatValue", "Double-64");return static_cast<Float>(readInt<8, std::int64_t>());}
    if (nextType == '\x01')     {ThorsMessage(5, "BsonParser", "getFloatValue", "Double-128");return static_cast<Float>(readFloat<double>());}
#if 0
    // TODO
    if (nextType == '\x13')     {return readFloat<long double>();}
#endif
    badType("Float", nextType);
}

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "BsonParser.source"
#endif

#endif
