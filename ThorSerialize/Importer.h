#ifndef THORS_ANVIL_SERIALIZE_IMPORTER_H
#define THORS_ANVIL_SERIALIZE_IMPORTER_H
/*
 * The Importer simply wrap an object of type T so that when de-serialized
 * it creates an object of type DeSerializer and calls it appropriately.
 */

#include "Serialize.h"

namespace ThorsAnvil
{
    namespace Serialize
    {

template<typename Format, typename T>
class Importer
{
    using ParserConfig = ParserInterface::ParserConfig;
    T&              value;
    ParserConfig    config;
    public:
        Importer(T& value, ParserConfig config = ParserConfig{})
            : value(value)
            , config(config)
        {}
        friend std::istream& operator>>(std::istream& stream, Importer const& data)
        {
            try
            {
                typename Format::Parser     parser(stream, data.config);
                DeSerializer                deSerializer(parser);

                deSerializer.parse(data.value);
            }
            catch (ThorsAnvil::Logging::CriticalException const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                // This exception is thrown because you are using deprecated code
                // that was not designed to be used with the bsonExporter
                // This must be fixed. So we are forcing a re-throw becuase
                // the generated binary object is probably bad.
                throw;
            }
            catch (std::exception const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                stream.setstate(std::ios::failbit);
                if (!data.config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", e.what());
                    throw;
                }
            }
            catch (...)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Importer", "operator>>", "UNKNOWN");
                stream.setstate(std::ios::failbit);
                if (!data.config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Importer", "operator>>", "UNKNOWN");
                    throw;
                }
            }
            return stream;
        }
};

template<typename Format, typename T>
Importer<Format, T> Import(T const& value, ParserInterface::ParserConfig config = ParserInterface::ParserConfig{})
{
    return Importer<Format, T>(value, config);
}


    }
}

#endif
