#ifndef THORS_ANVIL_SERIALIZE_YAML_H
#define THORS_ANVIL_SERIALIZE_YAML_H
/*
 * Defines the Yaml Serialization interface
 *      ThorsAnvil::Serialize::Yaml
 *      ThorsAnvil::Serialize::yamlExporter
 *      ThorsAnvil::Serialize::yamlImporter
 *
 * Usage:
 *      std::cout << yamlExporter(object); // converts object to Yaml on an output stream
 *      std::cin  >> yamlImporter(object); // converts Yaml to a C++ object from an input stream
 */


#include "YamlParser.h"
#include "YamlPrinter.h"
#include "Exporter.h"
#include "Importer.h"
#include "SerUtil.h"

namespace ThorsAnvil
{
    namespace Serialize
    {

struct Yaml
{
    using Parser  = YamlParser;
    using Printer = YamlPrinter;
};

// @function-api
// @param value                     The object to be serialized.
// @param config.characteristics    'Default': uses Config/Stream depending on global config. 'Config':  Is verbose and logical. 'Stream':  Remove all white space.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.   'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator<< for serialization.
template<typename T>
Exporter<Yaml, T> yamlExporter(T const& value, PrinterInterface::PrinterConfig config = PrinterInterface::PrinterConfig{})
{
    return Exporter<Yaml, T>(value, config);
}
template<typename T>
[[deprecated("Upgrade to use yamlExporter(). It has a more consistent interface. The difference is exceptions are caught by default and you need to manually turn the    m off. Turning the exceptions on/off is now part of the config object rahter than a seprate parameter.")]]
Exporter<Yaml, T> yamlExport(T const& value, PrinterInterface::PrinterConfig config = PrinterInterface::PrinterConfig{}, bool catchExceptions = false)
{
    config.catchExceptions = catchExceptions;
    return yamlExporter(value, config);
}
// @function-api
// @param value                     The object to be de-serialized.
// @param config.parseStrictness    'Weak':    ignore missing extra fields. 'Strict': Any missing or extra fields throws exception.
// @param config.polymorphicMarker  Jason object name for holding the polymorphic class name of the type. Default: __type
// @param config.catchExceptions    'false:    exceptions propogate.        'true':   parsing exceptions are stopped.
// @return                          Object that can be passed to operator>> for de-serialization.
template<typename T>
Importer<Yaml, T> yamlImporter(T& value, ParserInterface::ParserConfig config = ParserInterface::ParserConfig{})
{
    return Importer<Yaml, T>(value, config);
}
template<typename T>
[[deprecated("Upgrade to use yamlImporter(). It has a more consistent interface. The difference is exceptions are caught by default and you need to manually turn them off. Turning the exceptions on/off is now part of the config object rahter than a seprate parameter.")]]
Importer<Yaml, T> yamlImport(T& value, ParserInterface::ParserConfig config = ParserInterface::ParserConfig{}, bool catchExceptions = false)
{
    config.catchExceptions = catchExceptions;
    return yamlImporter(value, config);
}
    }
}

#endif
