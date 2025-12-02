#ifndef THORSANVIL_SERIALIZE_BSON_PRINTER_H
#define THORSANVIL_SERIALIZE_BSON_PRINTER_H
/*
 * BsonaryPrinter<T>
 *  See documentation in BsonParser.h
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include "MongoUtilityObjectId.h"
#include "BsonUtil.h"
#include <vector>
#include <iostream>
#include <functional>
#include <bit>

static_assert(
    std::endian::little == std::endian::native,
    "Don't want to support big endian unless I can do some good testing. Don't have a system for that so currently not suppoorted, but should be a relatively easy change"
);

namespace ThorsAnvil::Serialize
{

class BsonPrinter;
namespace MongoUtility
{
    class ObjectID;
    class UTCDateTime;
    BsonPrinter& operator<<(BsonPrinter& printer, MongoUtility::UTCDateTime const& data);
}

using IntTypes  = std::tuple<std::int32_t, std::int64_t>;
using IdStore   = std::optional<std::reference_wrapper<std::vector<MongoUtility::ObjectID>>>;

struct BsonPrinterConfig: public PrinterConfig
{
    IdStore    idStore;
    public:
        using PrinterConfig::PrinterConfig;
        BsonPrinterConfig(PrinterConfig val, IdStore idStore = {})
            : PrinterConfig(std::move(val))
            , idStore(idStore)
        {}
};

class BsonPrinter: public PrinterInterface
{
    friend BsonPrinter& MongoUtility::operator<<(BsonPrinter& printer, MongoUtility::UTCDateTime const& data);

    std::string                 currentKey;
    std::vector<BsonContainer>  currentContainer;
    std::vector<std::size_t>    arrayIndex;
    IdStore                     idStore;
    bool                        projection;
    private:
        std::size_t getSize(std::size_t expected) const             {return projection ? sizeof(int) : expected;}
        bool        writeProjection()                               {if (projection){writeInt<sizeof(int)>(1);}return projection;}
    public:
        BsonPrinter(std::ostream& output, BsonPrinterConfig config = BsonPrinterConfig{});
        BsonPrinter(std::string& output, BsonPrinterConfig config = BsonPrinterConfig{});
        virtual FormatType formatType()                             override {return FormatType::Bson;}
        virtual void openDoc()                                      override;
        virtual void closeDoc()                                     override;

        virtual void openMap(std::size_t size)                      override;
        virtual void closeMap()                                     override;
        virtual void openArray(std::size_t size)                    override;
        virtual void closeArray()                                   override;

        virtual void addKey(std::string_view const& key)            override;

        virtual void addValue(short int value)                      override    {if (writeProjection()){return;}writeInt<MaxTemplate<sizeof(short int), 4>::value>(value);}
        virtual void addValue(int value)                            override    {if (writeProjection()){return;}writeInt<sizeof(int)>(value);}
        virtual void addValue(long int value)                       override    {if (writeProjection()){return;}writeInt<sizeof(long int)>(value);}
        virtual void addValue(long long int value)                  override    {if (writeProjection()){return;}writeInt<sizeof(long long int)>(value);}
        virtual void addValue(unsigned short int value)             override    {if (writeProjection()){return;}writeInt<MaxTemplate<sizeof(unsigned short int), 4>::value>(value);}
        virtual void addValue(unsigned int value)                   override    {if (writeProjection()){return;}writeInt<sizeof(unsigned int)>(value);}
        virtual void addValue(unsigned long int value)              override    {if (writeProjection()){return;}writeInt<sizeof(unsigned long int)>(value);}
        virtual void addValue(unsigned long long int value)         override    {if (writeProjection()){return;}writeInt<sizeof(unsigned long long int)>(value);}

        virtual void addValue(float value)                          override    {if (writeProjection()){return;}writeFloat<double>(value);}
        virtual void addValue(double value)                         override    {if (writeProjection()){return;}writeFloat<double>(value);}
// TODO
// Currently long double is saved as ieee64 double precision.
// We need to work out how to use ieee128 quad precision where appropriate.
        virtual void addValue(long double value)                    override    {if (writeProjection()){return;}writeFloat<double>(value);}

        virtual void addValue(bool value)                           override    {if (writeProjection()){return;}writeBool(value);}

        virtual void addValue(std::string_view const& value)        override    {if (writeProjection()){return;}writeString(value);}

        virtual void addRawValue(std::string_view const& value)     override    {if (writeProjection()){return;}writeBinary(value);}

        virtual void addNull()                                      override    {if (writeProjection()){return;}writeNull();}
    protected:
        // Protected to allow unit tests
        virtual bool        printerUsesSize()                       override    {return true;}
        virtual void        pushLevel(bool isMap)                   override;
        virtual void        popLevel()                              override;
        virtual std::size_t getSizeMap(std::size_t count)           override;
        virtual std::size_t getSizeArray(std::size_t count)         override;
        virtual std::size_t getSizeMember(std::string_view const& m)override    {return std::size(m);}
        virtual std::size_t getSizeNull()                           override    {return getSize(0);}
        virtual std::size_t getSizeValue(short int)                 override    {return getSize(MaxTemplate<sizeof(short int), 4>::value);}
        virtual std::size_t getSizeValue(int)                       override    {return getSize(sizeof(int));}
        virtual std::size_t getSizeValue(long int)                  override    {return getSize(sizeof(long int));}
        virtual std::size_t getSizeValue(long long int)             override    {return getSize(sizeof(long long int));}
        virtual std::size_t getSizeValue(unsigned short int)        override    {return getSize(MaxTemplate<sizeof(unsigned short int), 4>::value);}
        virtual std::size_t getSizeValue(unsigned int)              override    {return getSize(sizeof(unsigned int));}
        virtual std::size_t getSizeValue(unsigned long int)         override    {return getSize(sizeof(unsigned long int));}
        virtual std::size_t getSizeValue(unsigned long long int)    override    {return getSize(sizeof(unsigned long long int));}
        virtual std::size_t getSizeValue(float)                     override    {return getSize(8);}
        virtual std::size_t getSizeValue(double)                    override    {return getSize(8);}
        virtual std::size_t getSizeValue(long double)               override    {return getSize(8);}
        virtual std::size_t getSizeValue(bool)                      override    {return getSize(1);}
        virtual std::size_t getSizeValue(std::string_view const& v) override    {return getSize(4 + std::size(v) + 1);}
        virtual std::size_t getSizeRaw(std::size_t size)            override    {return getSize(4 + 1 + size);}

    public:
        bool        setProjection(bool newValue)                                {return std::exchange(projection, newValue);}

        void writeKey(char value, std::size_t size);
        template<std::size_t size, typename Int> void writeLE(Int value)
        {
            Int docValue = value;
            write(reinterpret_cast<char*>(&docValue), size);
        }

        template<std::size_t size, typename Int> void writeBE(Int value)
        {
            Int     docValue = value;
            char*   docData  = reinterpret_cast<char*>(&docValue);
            for (std::size_t loop = 0; loop < sizeof(docValue)/2; ++loop) {
                std::swap(docData[loop], docData[sizeof(docValue) - loop - 1]);
            }
            write(docData + sizeof(docValue) - size, size);
        }


    private:
        template<std::size_t size, typename Int>
        void writeSize(Int value);
        template<std::size_t size, typename Int>
        void writeInt(Int value);
        template<typename Float>
        void writeFloat(Float value);
        void writeBool(bool value);
        void writeString(std::string_view const& value);
        void writeNull();
        void writeBinary(std::string_view const& value);

        bool needToInsertId() const;
};

template<std::size_t size, typename Int>
inline void BsonPrinter::writeSize(Int value)
{
    writeLE<size>(value);
}

template<std::size_t Size, typename Int>
inline void BsonPrinter::writeInt(Int value)
{
    static char  intKey[]      = {'\x10', '\x12'};
    using IntType = std::tuple_element_t<Size/4 - 1, IntTypes>;

    IntType             output = value;
    writeKey(intKey[Size/4 - 1], Size);
    writeLE<Size, IntType>(output);
}

template<typename F>
struct BsonPrintFloatTraits;

template<>
struct BsonPrintFloatTraits<float>
{
    static constexpr char           keyValue  = '\x01';
    static constexpr std::size_t    size      = 8;
    using ConversionType = double;
};
template<>
struct BsonPrintFloatTraits<double>
{
    static constexpr char           keyValue  = '\x01';
    static constexpr std::size_t    size      = 8;
    using ConversionType = double;
};
template<>
struct BsonPrintFloatTraits<long double>
{
    // TODO
    // Is there a supported 128 bit float?
    static constexpr char           keyValue  = '\x01';
    static constexpr std::size_t    size      = 8;
    using ConversionType = long double;
};
template<typename Float>
inline void BsonPrinter::writeFloat(Float value)
{
    typename BsonPrintFloatTraits<Float>::ConversionType outputValue = value;
    writeKey(BsonPrintFloatTraits<Float>::keyValue, BsonPrintFloatTraits<Float>::size);
    write(reinterpret_cast<char*>(&outputValue), BsonPrintFloatTraits<Float>::size);
}

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "BsonPrinter.source"
#endif

#endif
