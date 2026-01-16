#ifndef THORSANVIL_SERIALIZE_JSON_H
#define THORSANVIL_SERIALIZE_JSON_H
/*
 * Defines the Json Serialization interface
 *      ThorsAnvil::Serialize::Json
 *      ThorsAnvil::Serialize::jsonExporter
 *      ThorsAnvil::Serialize::jsonImporter
 *
 * Usage:
 *      std::cout << jsonExporter(object); // converts object to Json on an output stream
 *      std::cin  >> jsonImporter(object); // converts Json to a C++ object from an input stream
 */

#include "SerializeConfig.h"
#include "JsonParser.h"
#include "JsonPrinter.h"
#include "Exporter.h"
#include "Importer.h"
#include "ThorsSerializerUtil.h"

#include <iostream>
#include <ranges>
#include <utility>
#include <cstddef>

namespace ThorsAnvil::Serialize
{

struct Json
{
    using Parser  = JsonParser;
    using Printer = JsonPrinter;
};

// @function-api
// @param value                     The object to be serialized.
// @param config.characteristics    'Default': uses Config/Stream depending on global config. 'Config':  Is verbose and logical. 'Stream':  Remove all white space.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.   'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator<< for serialization.
template<typename T>
requires(Traits<T>::type != TraitType::Invalid)
Exporter<Json, T> jsonExporter(T const& value, PrinterConfig const& config = PrinterConfig{})
{
    return Exporter<Json, T>(value, config);
}
template<std::ranges::range R>
requires(Traits<R>::type == TraitType::Invalid)
ExporterRange<Json, R> jsonExporter(R range, PrinterConfig const& config = PrinterConfig{})
{
    return ExporterRange<Json, R>(std::move(range), config);
}

/*
 * Size of the output (if streamed with no extra space)
 */
template<typename T>
std::size_t jsonStreanSize(T const& value)
{
    ThorsAnvil::Serialize::JsonPrinter      printer(std::cout, PrinterConfig{OutputType::Stream});
    std::size_t size = ThorsAnvil::Serialize::Traits<T>::getPrintSize(printer, value, true);
    return size;
}
// @function-api
// @param value                     The object to be de-serialized.
// @param config.parseStrictness    'Weak':    ignore missing extra fields. 'Strict': Any missing or extra fields throws exception.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.        'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator>> for de-serialization.
template<typename T>
Importer<Json, T> jsonImporter(T& value, ParserConfig const& config = ParserConfig{})
{
    return Importer<Json, T>(value, config);
}
template<typename T, typename I>
T jsonBuilder(I&& stream, ParserConfig const& config = ParserConfig{})
{
    // Note: Stream can be std::istream / std::string / std::string_view
    T value;
    if (stream >> jsonImporter(value, config)) {
        return value;
    }
    return T{};
}

}

#endif
