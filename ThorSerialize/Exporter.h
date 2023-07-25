#ifndef THORSANVIL_SERIALIZE_EXPORTER_H
#define THORSANVIL_SERIALIZE_EXPORTER_H
/*
 * The Exporter simply wrap an object of type T so that when serialized
 * it creates an object of type Serializer and calls it appropriately.
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include "ThorsLogging/ThorsLogging.h"

namespace ThorsAnvil
{
    namespace Serialize
    {

template<typename Format, typename T, typename Config = PrinterInterface::PrinterConfig>
class Exporter
{
    T const&        value;
    Config          config;
    public:
        Exporter(T const& value, Config config)
            : value(value)
            , config(config)
        {}
        friend std::ostream& operator<<(std::ostream& stream, Exporter const& data)
        {
            try
            {
                typename Format::Printer    printer(stream, data.config);
                Serializer                  serializer(printer);

                serializer.print(data.value);
            }
            catch (ThorsAnvil::Logging::CriticalException const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                // This exception is thrown because you are using deprecated code
                // that was not designed to be used with the bsonExporter
                // This must be fixed. So we are forcing a re-throw becuase
                // the generated binary object is probably bad.
                throw;
            }
            catch (std::exception const& e)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                stream.setstate(std::ios::failbit);
                if (!data.config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                    throw;
                }
            }
            catch (...)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Exporter", "operator<<", "UNKNOWN");
                stream.setstate(std::ios::failbit);
                if (!data.config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator>>", "UNKNOWN");
                    throw;
                }
            }

            return stream;
        }
};

template<typename Format, typename T>
Exporter<Format, T> Export(T const& value, PrinterInterface::PrinterConfig config = PrinterInterface::PrinterConfig{})
{
    return Exporter<Format, T>(value, config);
}


    }
}

#endif
