#ifndef THORSANVIL_SERIALIZE_BSON_H
#define THORSANVIL_SERIALIZE_BSON_H
/*
 * Defines the Bson Serialization interface
 *      ThorsAnvil::Serialize::Bson
 *      ThorsAnvil::Serialize::bsonExporter
 *      ThorsAnvil::Serialize::bsonImporter
 *
 * Usage:
 *      std::cout << bsonExporter(object); // converts object to Bson on an output stream
 *      std::cin  >> bsonImporter(object); // converts Bson to a C++ object from an input stream
 */

#include "SerializeConfig.h"
#include "BsonParser.h"
#include "BsonPrinter.h"
#include "Exporter.h"
#include "Importer.h"

#include <type_traits>
#include <memory>
#include <stdexcept>
#include <ranges>
#include <utility>
#include <cstddef>
#include <sstream>

namespace ThorsAnvil::Serialize
{

template<typename T, TraitType trait = Traits<std::remove_cv_t<T>>::type>
struct BsonBaseTypeGetter
{
    static constexpr BsonContainer value = BsonContainer::Value;
    static void validate(T const&){}
};
template<typename T>
struct BsonBaseTypeGetter<T, TraitType::Map>
{
    static constexpr BsonContainer value = BsonContainer::Map;
    static void validate(T const&){}
};
template<typename T>
struct BsonBaseTypeGetter<T, TraitType::Parent>
{
    static constexpr BsonContainer value = BsonContainer::Map;
    static void validate(T const&){}
};
template<typename T>
struct BsonBaseTypeGetter<T, TraitType::Array>
{
    static constexpr BsonContainer value = BsonContainer::Array;
    static void validate(T const&){}
};
template<typename T>
struct BsonBaseTypeGetter<T, TraitType::Reference>
{
    static constexpr BsonContainer value = BsonContainer::Array;
    static void validate(T const&){}
};
template<typename T>
struct BsonBaseTypeGetter<T, TraitType::Pointer>
{
    using ElementType = typename std::pointer_traits<T>::element_type;
    static constexpr BsonContainer value = BsonBaseTypeGetter<ElementType>::value;
    static void validate(T const& pointer)
    {
        if (!pointer)
        {
            ThorsLogAndThrowError(std::runtime_error,
                                  "ThorsAnvil::Serialize::BsonBaseTypeGetter<T, Pointer>",
                                  "validate",
                                  "Bson does not support serialization of null at the top level");
        }
    }
};

struct Bson
{
    using Parser  = BsonParser;
    using Printer = BsonPrinter;
};

// @function-api
// @param value                     The object to be serialized.
// @param config.characteristics    'Default': uses Config/Stream depending on global config. 'Config':  Is verbose and logical. 'Stream':  Remove all white space.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.   'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator<< for serialization.
template<typename T>
requires(Traits<T>::type != TraitType::Invalid)
Exporter<Bson, T, BsonPrinterConfig, BsonPrinterConfig> bsonExporter(T const& value, PrinterConfig const& config = PrinterConfig{}, IdStore id = {})
{
    BsonBaseTypeGetter<T>::validate(value);

    return Exporter<Bson, T, BsonPrinterConfig, BsonPrinterConfig>(value, BsonPrinterConfig{config, BsonBaseTypeGetter<T>::value, id});
}
template<std::ranges::range R>
requires(Traits<R>::type == TraitType::Invalid)
ExporterRangeBson<Bson, R> bsonExporter(R range, PrinterConfig const& config = PrinterConfig{}, IdStore id = {})
{
    return ExporterRangeBson<Bson, R>(std::move(range), BsonPrinterConfig{config, BsonContainer::Array, id});
}

// @function-api
// @param value                     The object to be de-serialized.
// @param config.parseStrictness    'Weak':    ignore missing extra fields. 'Strict': Any missing or extra fields throws exception.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.        'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator>> for de-serialization.
template<typename T>
Importer<Bson, T, BsonParserConfig, BsonParserConfig> bsonImporter(T& value, ParserConfig const& config = ParserConfig{})
{
    return Importer<Bson, T, BsonParserConfig, BsonParserConfig>(value, BsonParserConfig{config, BsonBaseTypeGetter<T>::value});
}
template<typename T, typename I>
T bsonBuilder(I&& stream, ParserConfig const& config = ParserConfig{})
{
    // Note: Stream can be std::istream / std::string / std::string_view
    T value;
    if (stream >> bsonImporter(value, config)) {
        return value;
    }
    return T{};
}

// @function-api
// @param value                     The object to be serialized.
// @param config.characteristics    'Default': uses Config/Stream depending on global config. 'Config':  Is verbose and logical. 'Stream':  Remove all white space.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.   'true':   parsing exceptions are stopped.
// @return                          The size of the object that would be put on the stream in bytes.
template<typename T>
std::size_t bsonGetPrintSize(T const& value, PrinterConfig const& config = PrinterConfig{}, IdStore id = {})
{
    BsonBaseTypeGetter<T>::validate(value);

    std::stringstream         fakeStream;
    typename Bson::Printer    printer(fakeStream, BsonPrinterConfig{config, BsonBaseTypeGetter<T>::value, id});
    return Traits<std::remove_cv_t<T>>::getPrintSize(printer, value, false);
}

/*
 * BSON has the concept of a projection.
 * We specify what fields we want to retrieve.
 * The projection object achieves this.
 */
template<typename T>
struct Projection
{
    T                   projection;
};

template<typename T>
class Traits<Projection<T>>
{
    public:
        using RefType = T;
        struct ValueGetter
        {
            BsonPrinter&    printer;
            bool            originalValue;

            ValueGetter(PrinterInterface& p)
                : printer(dynamic_cast<BsonPrinter&>(p))
                , originalValue(printer.setProjection(true))

            {}
            ~ValueGetter()
            {
                printer.setProjection(originalValue);
            }

            T const&    getOutputValue(Projection<T> const& output) const   {return output.projection;}
        };
        static constexpr TraitType type = TraitType::Reference;
        static std::size_t getPrintSize(PrinterInterface& printer, Projection<T> const& object, bool p)
        {
            return Traits<std::remove_cv_t<T>>::getPrintSize(printer, object.projection, p);
        }
};

}

#endif
