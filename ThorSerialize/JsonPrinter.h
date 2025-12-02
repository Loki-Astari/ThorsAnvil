#ifndef THORSANVIL_SERIALIZE_JSON_PRINTER_H
#define THORSANVIL_SERIALIZE_JSON_PRINTER_H
/*
 * JsonaryPrinter<T>
 *  See documentation in JsonParser.h
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include <vector>
#include <cmath>

namespace ThorsAnvil::Serialize
{

// TODO This needs better field names
struct PrintState
{
    PrintState(int f0, TraitType f1, bool f2)
        : f0(f0), f1(f1), f2(f2)
    {}
    int         f0;
    TraitType   f1;
    bool        f2;
};
class JsonPrinter: public PrinterInterface
{
    std::vector<PrintState> state;
    public:
        JsonPrinter(std::ostream& output, PrinterConfig config = PrinterConfig{});
        JsonPrinter(std::string& output, PrinterConfig config = PrinterConfig{});

        virtual void reset()                                override;

        virtual FormatType formatType()                     override {return FormatType::Json;}
        virtual void openDoc()                              override;
        virtual void closeDoc()                             override;

        virtual void openMap(std::size_t size)              override;
        virtual void closeMap()                             override;
        virtual void openArray(std::size_t size)            override;
        virtual void closeArray()                           override;

        virtual void addKey(std::string_view const& key)    override;

        virtual void addValue(short int value)              override;
        virtual void addValue(int value)                    override;
        virtual void addValue(long int value)               override;
        virtual void addValue(long long int value)          override;

        virtual void addValue(unsigned short int value)     override;
        virtual void addValue(unsigned int value)           override;
        virtual void addValue(unsigned long int value)      override;
        virtual void addValue(unsigned long long int value) override;

        virtual void addValue(float value)                  override;
        virtual void addValue(double value)                 override;
        virtual void addValue(long double value)            override;

        virtual void addValue(bool value)                   override;

        virtual void addValue(std::string_view const& value)override;

        virtual void addRawValue(std::string_view const& value)  override;

        virtual void addNull()                              override;

        void addPrefix();

        virtual std::size_t getSizeMap(std::size_t count)           override {return 2 + (count * 2) - (count == 0 ? 0 : 1);}
        virtual std::size_t getSizeArray(std::size_t count)         override {return 2 + (count * 1) - (count == 0 ? 0 : 1);}
        virtual std::size_t getSizeMember(std::string_view const& m)override {return m.size() + 2;}
        virtual std::size_t getSizeNull()                           override {return 4;}
        virtual std::size_t getSizeValue(short int val)             override {return getOutputSizeInt(std::abs(val)) + (val < 0 ? 1 : 0);}
        virtual std::size_t getSizeValue(int val)                   override {return getOutputSizeInt(std::abs(val)) + (val < 0 ? 1 : 0);}
        virtual std::size_t getSizeValue(long int val)              override {return getOutputSizeInt(std::abs(val)) + (val < 0 ? 1 : 0);}
        virtual std::size_t getSizeValue(long long int val)         override {return getOutputSizeInt(std::abs(val)) + (val < 0 ? 1 : 0);}
        virtual std::size_t getSizeValue(unsigned short int val)    override {return getOutputSizeInt(val);}
        virtual std::size_t getSizeValue(unsigned int val)          override {return getOutputSizeInt(val);}
        virtual std::size_t getSizeValue(unsigned long int val)     override {return getOutputSizeInt(val);}
        virtual std::size_t getSizeValue(unsigned long long int val)override {return getOutputSizeInt(val);}
        virtual std::size_t getSizeValue(float val)                 override {return getOutputSizeFlt(val);}
        virtual std::size_t getSizeValue(double val)                override {return getOutputSizeFlt(val);}
        virtual std::size_t getSizeValue(long double val)           override {return getOutputSizeFlt(val);}
        virtual std::size_t getSizeValue(bool val)                  override {return val ? 4 : 5;}
        virtual std::size_t getSizeValue(std::string_view const& s) override {return s.size() + 2;}
        virtual std::size_t getSizeRaw(std::size_t val)             override {return getOutputSizeInt(val);}
    private:
        void addIndent();

        template<typename T>
        std::size_t getOutputSizeInt(T val)    {return val == 0 ? 1 : std::log10(val) + 1;}    // Integers
        template<typename T>
        std::size_t getOutputSizeFlt(T val)
        {
            if (val == 0) {
                return 3; // 0.0
            }
            static char buffer[100];
            auto result = std::to_chars(buffer, buffer + 100, val);
            return (result.ptr - buffer);
        }
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "JsonPrinter.source"
#endif

#endif
