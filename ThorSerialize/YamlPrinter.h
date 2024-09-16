#ifndef THORSANVIL_SERIALIZE_YAML_PRINTER_H
#define THORSANVIL_SERIALIZE_YAML_PRINTER_H

#include <functional>

#include "SerializeConfig.h"
#include "Serialize.h"
#include <yaml.h>
#include <vector>
#include <memory>

namespace ThorsAnvil::Serialize
{

class YamlPrinter: public PrinterInterface
{
    yaml_emitter_t    emitter;
    bool              error;
    std::vector<std::pair<int, TraitType>> state;

    void checkYamlResultCode(int code, char const* method, char const* msg);
    void checkYamlResultCode(std::function<int(yaml_event_t&)> init, char const* method, char const* msg);
    template<typename T>
    void emit(T const& data);
    void emitNull();

    void init();
    void complete();

    public:
        YamlPrinter(std::ostream& output, PrinterConfig config = PrinterConfig{});
        YamlPrinter(std::string& output, PrinterConfig config = PrinterConfig{});
        ~YamlPrinter();

        virtual void reset()                                override;

        virtual FormatType formatType()                     override {return FormatType::Json;}
        virtual void openDoc()                              override;
        virtual void closeDoc()                             override;
        virtual void openMap(std::size_t size)              override;
        virtual void closeMap()                             override;
        virtual void openArray(std::size_t size)            override;
        virtual void closeArray()                           override;

        virtual void addKey(std::string_view const& key)    override;

        virtual void addValue(short int value)              override    {emit(value);}
        virtual void addValue(int value)                    override    {emit(value);}
        virtual void addValue(long int value)               override    {emit(value);}
        virtual void addValue(long long int value)          override    {emit(value);}

        virtual void addValue(unsigned short int value)     override    {emit(value);}
        virtual void addValue(unsigned int value)           override    {emit(value);}
        virtual void addValue(unsigned long int value)      override    {emit(value);}
        virtual void addValue(unsigned long long int value) override    {emit(value);}

        virtual void addValue(float value)                  override    {emit(value);}
        virtual void addValue(double value)                 override    {emit(value);}
        virtual void addValue(long double value)            override    {emit(value);}

        virtual void addValue(bool value)                   override    {emit(value?"true":"false");}

        virtual void addValue(std::string_view const& value)override    {emit(std::string(value));}

        virtual void addRawValue(std::string_view const& value)override {emit(value);}

        virtual void addNull()                              override    {emitNull();}
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "YamlPrinter.source"
#endif

#endif
