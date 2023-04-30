#ifndef THORS_ANVIL_SERIALIZATION_CUSTOM_SERIALIZATION_H
#define THORS_ANVIL_SERIALIZATION_CUSTOM_SERIALIZATION_H

#include <cstdlib>

namespace ThorsAnvil
{
    namespace Serialize
    {

class PrinterInterface;
class ParserInterface;
class JsonPrinter;
class JsonParser;
class YamlPrinter;
class YamlParser;
class BsonPrinter;
class BsonParser;

template<typename T>
struct DefaultCustomSerializer
{
        virtual ~DefaultCustomSerializer() {}

        void writeCustom(PrinterInterface& printer, T const& object)            const;
        void readCustom(ParserInterface& parser, T& object)                     const;

        virtual void writeJson(JsonPrinter& printer, T const& object)           const;
        virtual void readJson(JsonParser& parser, T& object)                    const;
        virtual void writeYaml(YamlPrinter& printer, T const& object)           const;
        virtual void readYaml(YamlParser& parser, T& object)                    const;
        virtual std::size_t getPrintSizeBson(BsonPrinter& printer, T const& object) const;
        virtual char getBsonByteMark()                                          const{return '\x00';} // You should also override this see Bson specifications
        virtual void writeBson(BsonPrinter& printer, T const& object)           const;
        virtual void readBson(BsonParser& parser, char byteMarker, T& object)   const;
};
    }
}

#include "CustomSerialization.tpp"

#endif
