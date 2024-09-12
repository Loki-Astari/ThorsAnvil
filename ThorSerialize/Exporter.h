#ifndef THORSANVIL_SERIALIZE_EXPORTER_H
#define THORSANVIL_SERIALIZE_EXPORTER_H
/*
 * The Exporter simply wrap an object of type T so that when serialized
 * it creates an object of type Serializer and calls it appropriately.
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include "ThorsLogging/ThorsLogging.h"

namespace ThorsAnvil::Serialize
{

template<typename Format, typename T, typename Config = PrinterConfig>
class Exporter
{
    T const&        value;
    Config          config;
    public:
        Exporter(T const& value, Config config)
            : value(value)
            , config(config)
        {}
        template<typename F>
        bool inserter(F& stream) const
        {
            typename Format::Printer    printer(stream, config);
            try
            {
                Serializer                  serializer(printer);

                serializer.print(value);
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
                printer.setFail();
                if (!config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                    throw;
                }
            }
            catch (...)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Exporter", "operator<<", "UNKNOWN");
                printer.setFail();
                if (!config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator>>", "UNKNOWN");
                    throw;
                }
            }

            return printer.ok();
        }
        friend std::ostream& operator<<(std::ostream& stream, Exporter const& data)
        {
            data.inserter(stream);
            return stream;
        }
        friend bool operator<<(std::string& stream, Exporter const& data)
        {
            return data.inserter(stream);
        }
};

template<typename Format, typename T>
Exporter<Format, T> Export(T const& value, PrinterConfig config = PrinterConfig{})
{
    return Exporter<Format, T>(value, config);
}


}

#endif
