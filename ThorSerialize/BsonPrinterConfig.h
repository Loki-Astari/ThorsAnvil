#ifndef THORSANVIL_SERIALIZE_BSON_PRINTER_CONFIG_H
#define THORSANVIL_SERIALIZE_BSON_PRINTER_CONFIG_H

#include "PrinterConfig.h"
#include "MongoUtilityObjectId.h"
#include <functional>
#include <optional>

namespace ThorsAnvil::Serialize
{

using IdStore   = std::optional<std::reference_wrapper<std::vector<MongoUtility::ObjectID>>>;

struct BsonPrinterConfig: public PrinterConfig
{
    IdStore    idStore;
    public:
        using PrinterConfig::PrinterConfig;
        BsonPrinterConfig(PrinterConfig val, IdStore idStore = {})
            : PrinterConfig(std::move(val))
            , idStore(idStore)
        {}
};
}

#endif
