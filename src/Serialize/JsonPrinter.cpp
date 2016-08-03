
#include "JsonPrinter.h"

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
                char const** seporator = (!key && state.second == TraitType::Map)
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

JsonPrinter::JsonPrinter(std::ostream& output, OutputType characteristics)
    : PrinterInterface(output, characteristics)
{
    state.emplace_back(0, TraitType::Value);
}

void JsonPrinter::openDoc()
{}
void JsonPrinter::closeDoc()
{}

void JsonPrinter::openMap()
{
    output << PrefixMap(characteristics, state.size(), state.back()) << "{";
    state.emplace_back(0, TraitType::Map);
}
void JsonPrinter::closeMap()
{
    if (state.back().second != TraitType::Map)
    {
        throw std::runtime_error("ThorsAnvil::Serialize::JsonPrinter: Invalid call to closeMap(): Currently not in a map");
    }
    state.pop_back();
    output << PrefixMapClose(characteristics, state.size(), state.back()) << "}";
}
void JsonPrinter::openArray(std::size_t)
{
    output << PrefixArray(characteristics, state.size(), state.back()) << "[";
    state.emplace_back(0, TraitType::Array);
}
void JsonPrinter::closeArray()
{
    if (state.back().second != TraitType::Array)
    {
        throw std::runtime_error("ThorsAnvil::Serialize::JsonPrinter: Invalid call to closeArray(): Currently not in an array");
    }
    state.pop_back();
    output << PrefixArrayClose(characteristics, state.size(), state.back()) << "]";
}

void JsonPrinter::addKey(std::string const& key)
{
    if (state.back().second != TraitType::Map)
    {
        throw std::runtime_error("ThorsAnvil::Serialize::JsonPrinter: Invalid call to addKey(): Currently not in a map");
    }
    output << PrefixKey(characteristics, state.size(), state.back()) << '"' << key << '"';
}

void JsonPrinter::addValue(short value)                 {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(int value)                   {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(long value)                  {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(long long value)             {output << PrefixValue(characteristics, state.size(), state.back()) << value;}

void JsonPrinter::addValue(unsigned short value)        {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(unsigned int value)          {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(unsigned long value)         {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(unsigned long long value)    {output << PrefixValue(characteristics, state.size(), state.back()) << value;}

void JsonPrinter::addValue(float value)                 {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(double value)                {output << PrefixValue(characteristics, state.size(), state.back()) << value;}
void JsonPrinter::addValue(long double value)           {output << PrefixValue(characteristics, state.size(), state.back()) << value;}

void JsonPrinter::addValue(bool value)
{
    output << PrefixValue(characteristics, state.size(), state.back()) << std::boolalpha << value;
}
void JsonPrinter::addValue(std::string const& value)
{
    output << PrefixValue(characteristics, state.size(), state.back()) << '"' << value << '"';
}
