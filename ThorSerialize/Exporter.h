#ifndef THORSANVIL_SERIALIZE_EXPORTER_H
#define THORSANVIL_SERIALIZE_EXPORTER_H
/*
 * The Exporter simply wrap an object of type T so that when serialized
 * it creates an object of type Serializer and calls it appropriately.
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include "ThorsLogging/ThorsLogging.h"
#include "BsonPrinterConfig.h"

namespace ThorsAnvil::Serialize
{

template<typename Format, typename Config = PrinterConfig>
class ExporterBase
{
    Config         config;
    public:
        ExporterBase(Config&& config)
            : config(std::move(config))
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
                if (!config.catchUnknownExceptions)
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

template<typename Format, typename T, typename Config = PrinterConfig>
class Exporter: public ExporterBase<Format, Config>
{
    T const&        value;
    public:
        Exporter(T const& value, Config&& config)
            : ExporterBase<Format, Config>(std::move(config))
            , value(value)
        {}
        virtual void doInserter(PrinterInterface& printer) const
        {
            Serializer                  serializer(printer);
            serializer.print(value);
        }
};

template<typename Format, std::ranges::range R, typename Config = PrinterConfig>
class ExporterRange: public ExporterBase<Format, Config>
{
    mutable R           range;
    public:
        ExporterRange(R&& range, Config&& config)
            : ExporterBase<Format, Config>(std::move(config))
            , range(std::move(range))
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
class ExporterRangeBson: public ExporterBase<Format, BsonPrinterConfig>
{
    mutable R       range;
    public:
        ExporterRangeBson(R&& range, BsonPrinterConfig&& config)
            : ExporterBase<Format, BsonPrinterConfig>(std::move(config))
            , range(std::move(range))
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
