#ifndef THORSANVIL_SERIALIZATION_CUSTOM_SERIALIZATION_H
#define THORSANVIL_SERIALIZATION_CUSTOM_SERIALIZATION_H

#include "SerializeConfig.h"
#include "Serialize.h"

#include <cstddef>

namespace ThorsAnvil::Serialize
{

class PrinterInterface;
class ParserInterface;
class JsonPrinter;
class JsonParser;
class YamlPrinter;
class YamlParser;
class BsonPrinter;
class BsonParser;

/*
 * This is an example Custom Serilization class.
 * See: ThorsAnvil_MakeTraitCustomSerialize() macro in Traits.h
 *
 * This is used to maintain backward compatability in the test.
 * You can inherit from this class to maintain backward compatability after a change on May/2026
 *
 * But if you are implementing this for the first time you don't need to inherit from this class
 * You simply need a class that implements:
 *
 *      => void readCustom(DeSerializer& parent, ParserInterface& parser, T& object)
 *      => void writeCustom(Serializer& parent, PrinterInterface& printer, T consT& object)
 *
 * Note the code uses duck-typing so these don't need to be virtual.
 */
template<typename T>
struct CustomSerializerBreakerFix
{
    virtual ~CustomSerializerBreakerFix() {}
    virtual void readCustom(DeSerializer& /*parent*/, ParserInterface& parser, T& object) const
    {
        ParserToken    tokenType = parser.getToken();
        if (tokenType != ParserToken::Value)
        {
            ThorsLogAndThrowError(std::runtime_error,
                                      "ThorsAnvil::Serialize::DeSerializationForBlock<Value>",
                                      "DeSerializationForBlock",
                                      "Invalid Object");
        }
        readCustomOld(parser, object);
    }
    virtual void readCustomOld(ParserInterface& parser, T& object) const = 0;

    virtual void writeCustom(Serializer& /*parent*/, PrinterInterface& printer, T const& object) const
    {
        writeCustomOld(printer, object);
    }
    virtual void writeCustomOld(PrinterInterface& printer, T const& object) const = 0;

    virtual std::size_t getPrintSize(PrinterInterface& printer, T const& object) const
    {
        switch (printer.formatType())
        {
            case FormatType::Bson:  return getPrintSizeBson(dynamic_cast<BsonPrinter&>(printer), object);
            case FormatType::Json:  return getPrintSizeJson(printer, object);
            case FormatType::Yaml:  /* Fall Through */
            default:
            {
                ThorsLogAndThrowError(std::runtime_error,
                                      "ThorsAnvil::Serialize::Traits<DataType>",
                                      "getPrintSize",
                                      "Should not get here");
            }
        }
    }
    virtual std::size_t getPrintSizeBson(BsonPrinter& printer, T const& object) const = 0;
    virtual std::size_t getPrintSizeJson(PrinterInterface& printer, T const& object) const = 0;
};

template<typename T>
struct DefaultCustomSerializer: public CustomSerializerBreakerFix<T>
{
        virtual ~DefaultCustomSerializer() {}

        virtual void writeCustomOld(PrinterInterface& printer, T const& object)    const override;
        virtual void readCustomOld(ParserInterface& parser, T& object)             const override;

        virtual void writeJson(JsonPrinter& printer, T const& object)           const;
        virtual void readJson(JsonParser& parser, T& object)                    const;
        virtual void writeYaml(YamlPrinter& printer, T const& object)           const;
        virtual void readYaml(YamlParser& parser, T& object)                    const;

        virtual char getBsonByteMark()                                          const{return '\x00';} // You should also override this see Bson specifications
        virtual void writeBson(BsonPrinter& printer, T const& object)           const;
        virtual void readBson(BsonParser& parser, char byteMarker, T& object)   const;

        virtual std::size_t getPrintSizeBson(BsonPrinter& printer, T const& object) const override;
        virtual std::size_t getPrintSizeJson(PrinterInterface& printer, T const& object) const override;
};

}

#include "CustomSerialization.tpp"

#endif
