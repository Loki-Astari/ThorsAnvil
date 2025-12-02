#ifndef THORSANVIL_SERIALIZER_PRINTER_CONFIG_H
#define THORSANVIL_SERIALIZER_PRINTER_CONFIG_H

#include "SerializeConfig.h"
#include "PolymorphicMarker.h"
#include <string>

namespace ThorsAnvil::Serialize
{

// Default:     What ever the implementation likes.
// Stream:      Compressed for over the wire protocol.
// Config:      Human readable (potentially config file like)
enum class OutputType {Default, Stream, Config};


struct PrinterConfig
{
    /*
     * These constructors are maintained for backward compatibility.
     * But should not be used in new code.
     * Please use the set<Attribute>() methods.
     */
    PrinterConfig(OutputType characteristics,
                  std::string const& polymorphicMarker = "",
                  bool catchExceptions = true)
        : characteristics(characteristics)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    PrinterConfig(std::string const& polymorphicMarker,
                  bool catchExceptions = true)
        : polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    PrinterConfig(bool catchExceptions)
        : catchExceptions(catchExceptions)
    {}
    PrinterConfig(OutputType characteristic, bool catchExceptions)
        : characteristics(characteristic)
        , catchExceptions(catchExceptions)
    {}

    /* Please use the default constructor
     * Then call the appropriate set<Attributes>() to define the characteristics you need.
     */
    PrinterConfig()
        : characteristics(OutputType::Default)
    {}
    PrinterConfig& setOutputType(OutputType p_characteristics)                  {characteristics = p_characteristics;      return *this;}
    PrinterConfig& setPolymorphicMarker(std::string const& p_polymorphicMarker) {polymorphicMarker = p_polymorphicMarker;  return *this;}
    PrinterConfig& setCatchExceptions(bool p_catchExceptions)                   {catchExceptions = p_catchExceptions;      return *this;}
    PrinterConfig& setCatchUnknownExceptions(bool p_catchExceptions)            {catchUnknownExceptions =p_catchExceptions;return *this;}
    PrinterConfig& setUseOldSharedPtr()                                         {useOldSharedPtr = true;                   return *this;}
    PrinterConfig& setExactPreFlightCalc()                                      {exactPreFlight = true;                    return *this;}

    OutputType      characteristics         = OutputType::Default;
    std::string     polymorphicMarker       = "";
    bool            catchExceptions         = true;
    bool            catchUnknownExceptions  = false;
    long            parserInfo              = 0;
    bool            useOldSharedPtr         = false;
    bool            exactPreFlight          = false;
};


}

#endif
