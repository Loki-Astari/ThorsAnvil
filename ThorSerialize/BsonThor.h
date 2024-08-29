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
#include "SerUtil.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"

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
            ThorsLogAndThrow("ThorsAnvil::Serialize::BsonBaseTypeGetter<T, Pointer>",
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
Exporter<Bson, T, BsonPrinterConfig> bsonExporter(T const& value, BsonPrinterConfig config = PrinterConfig{})
{
    config.parserInfo = static_cast<long>(BsonBaseTypeGetter<T>::value);
    BsonBaseTypeGetter<T>::validate(value);

    return Exporter<Bson, T, BsonPrinterConfig>(value, config);
}

// @function-api
// @param value                     The object to be de-serialized.
// @param config.parseStrictness    'Weak':    ignore missing extra fields. 'Strict': Any missing or extra fields throws exception.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.        'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator>> for de-serialization.
template<typename T>
Importer<Bson, T> bsonImporter(T& value, ParserConfig config = ParserConfig{})
{
    config.parserInfo = static_cast<long>(BsonBaseTypeGetter<T>::value);

    return Importer<Bson, T>(value, config);
}

// @function-api
// @param value                     The object to be serialized.
// @param config.characteristics    'Default': uses Config/Stream depending on global config. 'Config':  Is verbose and logical. 'Stream':  Remove all white space.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.   'true':   parsing exceptions are stopped.
// @return                          The size of the object that would be put on the stream in bytes.
template<typename T>
std::size_t bsonGetPrintSize(T const& value, BsonPrinterConfig config = PrinterConfig{})
{
    config.parserInfo = static_cast<long>(BsonBaseTypeGetter<T>::value);
    BsonBaseTypeGetter<T>::validate(value);

    std::stringstream         fakeStream;
    typename Bson::Printer    printer(fakeStream, config);
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
