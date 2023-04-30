#ifndef THORS_ANVIL_SERIALIZE_BSON_PRINTER_H
#define THORS_ANVIL_SERIALIZE_BSON_PRINTER_H
/*
 * BsonaryPrinter<T>
 *  See documentation in BsonParser.h
 */

#include "Serialize.h"
#include "BsonUtil.h"
#include "GitUtility/ieee754_types.h"
#include <boost/endian/conversion.hpp>
#include <vector>
#include <iostream>

namespace ThorsAnvil
{
    namespace Serialize
    {
        class BsonPrinter;
        namespace MongoUtility
        {
            class UTCDateTime;
            BsonPrinter& operator<<(BsonPrinter& printer, MongoUtility::UTCDateTime const& data);
        }

using IntTypes = std::tuple<std::int32_t, std::int64_t>;
extern char  intKey[];
extern char  floatKey[];

class BsonPrinter: public PrinterInterface
{
    friend BsonPrinter& MongoUtility::operator<<(BsonPrinter& printer, MongoUtility::UTCDateTime const& data);

    std::string currentKey;
    std::vector<BsonContainer>  currentContainer;
    std::vector<std::size_t>    arrayIndex;
    public:
        BsonPrinter(std::ostream& output, PrinterConfig config = PrinterConfig{});
        virtual FormatType formatType()                             override {return FormatType::Bson;}
        virtual void openDoc()                                      override;
        virtual void closeDoc()                                     override;

        virtual void openMap(std::size_t size)                      override;
        virtual void closeMap()                                     override;
        virtual void openArray(std::size_t size)                    override;
        virtual void closeArray()                                   override;

        virtual void addKey(std::string const& key)                 override;

        virtual void addValue(short int value)                      override    {writeInt<MaxTemplate<sizeof(short int), 4>::value>(value);}
        virtual void addValue(int value)                            override    {writeInt<sizeof(int)>(value);}
        virtual void addValue(long int value)                       override    {writeInt<sizeof(long int)>(value);}
        virtual void addValue(long long int value)                  override    {writeInt<sizeof(long long int)>(value);}
        virtual void addValue(unsigned short int value)             override    {writeInt<MaxTemplate<sizeof(unsigned short int), 4>::value>(value);}
        virtual void addValue(unsigned int value)                   override    {writeInt<sizeof(unsigned int)>(value);}
        virtual void addValue(unsigned long int value)              override    {writeInt<sizeof(unsigned long int)>(value);}
        virtual void addValue(unsigned long long int value)         override    {writeInt<sizeof(unsigned long long int)>(value);}

        virtual void addValue(float value)                          override    {writeFloat<MaxTemplate<sizeof(float), 8>::value>(value);}
        virtual void addValue(double value)                         override    {writeFloat<sizeof(double)>(value);}
// Work here
// Currently long double is saved as ieee64 double precision.
// We need to work out how to use ieee128 quad precision where appropriate.
        virtual void addValue(long double value)                    override    {writeFloat<8>(value);}

        virtual void addValue(bool value)                           override    {writeBool(value);}

        virtual void addValue(std::string const& value)             override    {writeString(value);}
        virtual void addValue(std::string_view const& value)        override    {writeString(std::string(value));}

        virtual void addRawValue(std::string const& value)          override    {writeBinary(value);}

        virtual void addNull()                                      override    {writeNull();}
    protected:
        // Protected to allow unit tests
        virtual bool        printerUsesSize()                       override    {return true;}
        virtual std::size_t getSizeMap(std::size_t /*count*/)       override;
        virtual std::size_t getSizeArray(std::size_t /*count*/)     override;
        virtual std::size_t getSizeNull()                           override    {return 0;}
        virtual std::size_t getSizeValue(short int)                 override    {return MaxTemplate<sizeof(short int), 4>::value;}
        virtual std::size_t getSizeValue(int)                       override    {return sizeof(int);}
        virtual std::size_t getSizeValue(long int)                  override    {return sizeof(long int);}
        virtual std::size_t getSizeValue(long long int)             override    {return sizeof(long long int);}
        virtual std::size_t getSizeValue(unsigned short int)        override    {return MaxTemplate<sizeof(unsigned short int), 4>::value;}
        virtual std::size_t getSizeValue(unsigned int)              override    {return sizeof(unsigned int);}
        virtual std::size_t getSizeValue(unsigned long int)         override    {return sizeof(unsigned long int);}
        virtual std::size_t getSizeValue(unsigned long long int)    override    {return sizeof(unsigned long long int);}
        virtual std::size_t getSizeValue(float)                     override    {return 8;}
        virtual std::size_t getSizeValue(double)                    override    {return 8;}
        virtual std::size_t getSizeValue(long double)               override    {return 8;}
        virtual std::size_t getSizeValue(bool)                      override    {return 1;}
        virtual std::size_t getSizeValue(std::string const& value)  override    {return 4 + value.size() + 1;}
        virtual std::size_t getSizeValue(std::string_view const& v) override    {return 4 + std::size(v) + 1;}
        virtual std::size_t getSizeRaw(std::size_t size)            override    {return 4 + 1 + size;}

    public:
        void writeKey(char value, std::size_t size);
        template<std::size_t size, typename Int> void writeLE(Int value)
        {
            Int docValue = boost::endian::native_to_little(value);
            output.write(reinterpret_cast<char*>(&docValue), size);
        }

        template<std::size_t size, typename Int> void writeBE(Int value)
        {
            Int docValue = boost::endian::native_to_big(value);
            output.write(reinterpret_cast<char*>(&docValue) + (sizeof(docValue) - size), size);
        }


    private:
        template<std::size_t size, typename Int>
        void writeSize(Int value);
        template<std::size_t size, typename Int>
        void writeInt(Int value);
        template<std::size_t size, typename Float>
        void writeFloat(Float value);
        void writeBool(bool value);
        void writeString(std::string const& value);
        void writeNull();
        void writeBinary(std::string const& value);

};

template<std::size_t size, typename Int>
inline void BsonPrinter::writeSize(Int value)
{
    writeLE<size>(value);
}

template<std::size_t Size, typename Int>
inline void BsonPrinter::writeInt(Int value)
{
    using IntType = typename std::tuple_element<Size/4 - 1, IntTypes>::type;

    IntType             output = value;
    writeKey(intKey[Size/4 - 1], Size);
    writeLE<Size, IntType>(output);
}

template<std::size_t Size, typename Float>
inline void BsonPrinter::writeFloat(Float value)
{
    IEEE_754::_2008::Binary<Size * 8>   outputValue = value;
    writeKey(floatKey[Size/8 - 1], Size);
    output.write(reinterpret_cast<char*>(&outputValue), Size);
}

    }
}

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "BsonPrinter.source"
#endif

#endif
