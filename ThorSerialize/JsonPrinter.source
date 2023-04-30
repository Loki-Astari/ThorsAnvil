#include "SerializeConfig.h"
#include "JsonPrinter.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <iomanip>
#include <algorithm>

using namespace ThorsAnvil::Serialize;

namespace
{
    struct Prefix
    {

        static char const*  space[];
        static char const*  comma[];
        static char const*  colon[];

        PrinterInterface::OutputType    characteristics;
        int                             size;
        std::pair<int, TraitType>&      state;
        public:
            Prefix(PrinterInterface::OutputType characteristics, int size, std::pair<int, TraitType>& state)
                : characteristics(characteristics)
                , size(size)
                , state(state)
            {}
            void printSeporator(std::ostream& stream, bool key) const
            {
                char const*(&seporator)[] = (!key && state.second == TraitType::Map)
                                            ? colon
                                            : (state.first != 0) ? comma : space;
                stream << seporator[static_cast<int>(characteristics)];
            }
    };
    struct PrefixKey: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixKey const& data)
        {
            data.printSeporator(stream, true);
            if (data.characteristics == PrinterInterface::OutputType::Stream)
            {
                return stream;
            }
            return stream << "\n" << std::string(data.size, '\t');
        }
    };
    struct PrefixValue: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixValue const& data)
        {
            data.printSeporator(stream, false);
            ++data.state.first;

            //if (data.characteristics == PrinterInterface::OutputType::Stream || data.state.second == TraitType::Array)
            {
                return stream;
            }
            //return stream << "\n" << std::string(data.size, '\t');
        }
    };
    struct PrefixMap: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixMap const& data)
        {
            data.printSeporator(stream, false);

            if (data.characteristics == PrinterInterface::OutputType::Stream)
            {
                return stream;
            }
            return stream << "\n" << std::string(data.size, '\t');
        }
    };
    struct PrefixMapClose: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixMapClose const& data)
        {
            ++data.state.first;
            if (data.characteristics == PrinterInterface::OutputType::Stream)
            {
                return stream;
            }
            return stream << "\n" << std::string(data.size, '\t');
        }
    };
    struct PrefixArray: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixArray const& data)
        {
            data.printSeporator(stream, false);
            return stream;
        }
    };
    struct PrefixArrayClose: public Prefix
    {
        using Prefix::Prefix;
        friend std::ostream& operator<<(std::ostream& stream, PrefixArrayClose const& data)
        {
            ++data.state.first;
            return stream;
        }
    };
}

char const*  Prefix::space[]   = {" ",  "",  " "};
char const*  Prefix::comma[]   = {", ", ",", ", "};
char const*  Prefix::colon[]   = {": ", ":", ": "};

HEADER_ONLY_INCLUDE
JsonPrinter::JsonPrinter(std::ostream& output, PrinterConfig config)
    : PrinterInterface(output, config)
{
    state.emplace_back(0, TraitType::Value);
}

HEADER_ONLY_INCLUDE
void JsonPrinter::openDoc()
{}
HEADER_ONLY_INCLUDE
void JsonPrinter::closeDoc()
{}

HEADER_ONLY_INCLUDE
void JsonPrinter::openMap(std::size_t)
{
    output << PrefixMap(config.characteristics, state.size(), state.back()) << "{";
    state.emplace_back(0, TraitType::Map);
}
HEADER_ONLY_INCLUDE
void JsonPrinter::closeMap()
{
    if (state.back().second != TraitType::Map)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::JsonPrinter",
                         "closeMap",
                         "Invalid call to closeMap(): Currently not in a map");
    }
    state.pop_back();
    output << PrefixMapClose(config.characteristics, state.size(), state.back()) << "}";
}
HEADER_ONLY_INCLUDE
void JsonPrinter::openArray(std::size_t)
{
    output << PrefixArray(config.characteristics, state.size(), state.back()) << "[";
    state.emplace_back(0, TraitType::Array);
}
HEADER_ONLY_INCLUDE
void JsonPrinter::closeArray()
{
    if (state.back().second != TraitType::Array)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::JsonPrinter",
                         "closeArray",
                         "Invalid call to closeArray(): Currently not in an array");
    }
    state.pop_back();
    output << PrefixArrayClose(config.characteristics, state.size(), state.back()) << "]";
}

HEADER_ONLY_INCLUDE
void JsonPrinter::addKey(std::string const& key)
{
    if (state.back().second != TraitType::Map)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize::JsonPrinter",
                         "addKey",
                         "Invalid call to addKey(): Currently not in a map");
    }
    output << PrefixKey(config.characteristics, state.size(), state.back()) << '"' << key << '"';
}

HEADER_ONLY_INCLUDE
void JsonPrinter::addPrefix()
{
    output << PrefixValue(config.characteristics, state.size(), state.back());
}

template<typename T>
struct FormatDouble
{
    T const &  value;
    FormatDouble(T const& value): value(value){}
    friend std::ostream& operator<<(std::ostream& str, FormatDouble const& formater)
    {
        if (formater.value == 0)
        {
            return str << "0.0";
        }
        else
        {
            return str << formater.value;
        }
    }
};

HEADER_ONLY_INCLUDE void JsonPrinter::addValue(short int value)             {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(int value)                   {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(long int value)              {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(long long int value)         {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}

HEADER_ONLY_INCLUDE void JsonPrinter::addValue(unsigned short int value)    {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(unsigned int value)          {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(unsigned long int value)     {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(unsigned long long int value){output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}

HEADER_ONLY_INCLUDE void JsonPrinter::addValue(float value)                 {output << PrefixValue(config.characteristics, state.size(), state.back()) << FormatDouble<float>(value);}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(double value)                {output << PrefixValue(config.characteristics, state.size(), state.back()) << FormatDouble<double>(value);}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(long double value)           {output << PrefixValue(config.characteristics, state.size(), state.back()) << FormatDouble<long double>(value);}

HEADER_ONLY_INCLUDE void JsonPrinter::addValue(bool value)                  {output << PrefixValue(config.characteristics, state.size(), state.back()) << std::boolalpha << value;}

HEADER_ONLY_INCLUDE void JsonPrinter::addValue(std::string const& value)    {output << PrefixValue(config.characteristics, state.size(), state.back()) << '"' << EscapeString(value) << '"';}
HEADER_ONLY_INCLUDE void JsonPrinter::addValue(std::string_view const&value){output << PrefixValue(config.characteristics, state.size(), state.back()) << '"' << EscapeString(value) << '"';}
HEADER_ONLY_INCLUDE void JsonPrinter::addRawValue(std::string const& value) {output << PrefixValue(config.characteristics, state.size(), state.back()) << value;}

HEADER_ONLY_INCLUDE void JsonPrinter::addNull()                             {output << PrefixValue(config.characteristics, state.size(), state.back()) << "null";}
