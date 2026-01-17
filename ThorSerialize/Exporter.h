#ifndef THORSANVIL_SERIALIZE_EXPORTER_H
#define THORSANVIL_SERIALIZE_EXPORTER_H
/*
 * The Exporter simply wrap an object of type T so that when serialized
 * it creates an object of type Serializer and calls it appropriately.
 */

#include "SerializeConfig.h"
#include "ThorsSerializerUtil.h"
#include "Serialize.h"

#include <concepts>
#include <string>
#include <ostream>
#include <ranges>
#include <functional>
#include <exception>
#include <utility>
#include <cstddef>
#include <type_traits>

namespace ThorsAnvil::Serialize
{

struct BsonPrinterConfig;

template<typename Format, typename Config, typename ConfigStore>
class ExporterBase
{
    ConfigStore  config;
    public:
        ExporterBase(Config const& config)
            : config(config)
        {}
        virtual void doInserter(PrinterInterface& printer) const = 0;
        template<typename F>
        bool inserter(F& stream) const
        {
            typename Format::Printer    printer(stream, config);
            try
            {
                doInserter(printer);
            }
            catch (DepricatedIssue const& e)
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
                if (!printer.config.catchExceptions)
                {
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator<<", e.what());
                    throw;
                }
            }
            catch (...)
            {
                ThorsCatchMessage("ThorsAnvil::Serialize::Exporter", "operator<<", "UNKNOWN");
                printer.setFail();
                if (!printer.config.catchUnknownExceptions)
                {
                    // Don't set this to true if you are using boost-coroutines.
                    // As this may prevent the force_unwind exception propagating.
                    ThorsRethrowMessage("ThorsAnvil::Serialize::Exporter", "operator>>", "UNKNOWN");
                    throw;
                }
            }

            return printer.ok();
        }
        friend std::ostream& operator<<(std::ostream& stream, ExporterBase const& data)
        {
            data.inserter(stream);
            return stream;
        }
        friend bool operator<<(std::string& stream, ExporterBase const& data)
        {
            return data.inserter(stream);
        }
};

template<typename Format, typename T, typename Config = PrinterConfig, typename ConfigStore = std::reference_wrapper<const Config>>
class Exporter: public ExporterBase<Format, Config, ConfigStore>
{
    T const&        value;
    public:
        Exporter(T const& value, Config const& config)
            : ExporterBase<Format, Config, ConfigStore>(config)
            , value(value)
        {}
        virtual void doInserter(PrinterInterface& printer) const
        {
            Serializer                  serializer(printer);
            serializer.print(value);
        }
};

template<typename Format, std::ranges::range R, typename Config = PrinterConfig, typename ConfigStore = std::reference_wrapper<const Config>>
class ExporterRange: public ExporterBase<Format, Config, ConfigStore>
{
    R const&        range;
    public:
        ExporterRange(R const& range, Config const& config)
            : ExporterBase<Format, Config, ConfigStore>(config)
            , range(range)
        {}
        virtual void doInserter(PrinterInterface& printer) const override
        {
            Serializer          serializer(printer);
            //printer.openDoc();
            printer.openArray(0);
            for (auto const& item: range) {
                serializer.print(item);
            }
            printer.closeArray();
            //printer.closeDoc();
        }
};

template<typename Format, std::ranges::sized_range R>
requires (Traits<R>::type == TraitType::Invalid)
class ExporterRangeBson: public ExporterBase<Format, BsonPrinterConfig, BsonPrinterConfig>
{
    R const&        range;
    public:
        ExporterRangeBson(R const& range, BsonPrinterConfig const& config)
            : ExporterBase<Format, BsonPrinterConfig, BsonPrinterConfig>(config)
            , range(range)
        {}
        virtual void doInserter(PrinterInterface& printer) const override
        {
            printer.openDoc();

            std::size_t size = printer.getSizeArray(std::size(range));
            using VT = std::ranges::range_value_t<R>;
            printer.pushLevel(false);
            for (auto const& val: range) {
                size += Traits<std::remove_cv_t<VT>>::getPrintSize(printer, val, false);
            }
            printer.popLevel();


            Serializer          serializer(printer);
            printer.openArray(size);
            for (auto const& val: range) {
                serializer.print(val);
            }
            printer.closeArray();
            printer.closeDoc();
            printer.finalizePrint();
        }
};

template<typename Format, typename T>
Exporter<Format, T> Export(T const& value, PrinterConfig config = PrinterConfig{})
{
    return Exporter<Format, T>(value, config);
}


}

#endif
