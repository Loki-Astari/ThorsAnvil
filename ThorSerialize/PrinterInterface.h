#ifndef THORSANVIL_SERIALIZER_PRINTER_INTERFACE_H
#define THORSANVIL_SERIALIZER_PRINTER_INTERFACE_H

#include "SerializeConfig.h"
#include "PrinterConfig.h"
#include "StringOutput.h"
#include "ThorsSerializerUtilTypes.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <variant>
#include <iterator>
#include <memory>

namespace ThorsAnvil::Serialize
{

class PrinterInterface
{
    public:
        PrinterConfig const  config;

        PrinterInterface(std::ostream& output, PrinterConfig config = PrinterConfig{})
            : config(config)
            , output(&output)
        {}
        PrinterInterface(std::string& output, PrinterConfig config = PrinterConfig{})
            : config(config)
            , output(StringOutput{output, true /*!config.exactPreFlight*/})
        {}

                bool    preflightSize();
                void    reserveSize();
                void    finalizePrint();
        virtual void    reset()     {}

        virtual ~PrinterInterface() {}
        virtual FormatType formatType()                 = 0;
        virtual void    openDoc()                       = 0;
        virtual void    closeDoc()                      = 0;
        virtual void    openMap(std::size_t size)       = 0;
        virtual void    closeMap()                      = 0;
        virtual void    openArray(std::size_t size)     = 0;
        virtual void    closeArray()                    = 0;

        virtual void    addKey(std::string_view const& key)  = 0;

        virtual void    addValue(short int)             = 0;
        virtual void    addValue(int)                   = 0;
        virtual void    addValue(long int)              = 0;
        virtual void    addValue(long long int)         = 0;

        virtual void    addValue(unsigned short int)    = 0;
        virtual void    addValue(unsigned int)          = 0;
        virtual void    addValue(unsigned long int)     = 0;
        virtual void    addValue(unsigned long long int)= 0;

        virtual void    addValue(float)                 = 0;
        virtual void    addValue(double)                = 0;
        virtual void    addValue(long double)           = 0;

        virtual void    addValue(bool)                  = 0;

        virtual void    addValue(std::string_view const&) = 0;

        virtual void    addRawValue(std::string_view const&) = 0;

        virtual void    addNull()                       = 0;

        void addValue(void*)        = delete;
        void addValue(void const*)  = delete;

        virtual bool        printerUsesSize()                       {return false;}
        virtual void        pushLevel(bool)                         {}
        virtual void        popLevel()                              {}
        virtual std::size_t getSizeMap(std::size_t /*count*/)       {return 0;}
        virtual std::size_t getSizeArray(std::size_t /*count*/)     {return 0;}
        virtual std::size_t getSizeMember(std::string_view const&)  {return 0;}
        virtual std::size_t getSizeNull()                           {return 0;}
        virtual std::size_t getSizeValue(short int)                 {return 0;}
        virtual std::size_t getSizeValue(int)                       {return 0;}
        virtual std::size_t getSizeValue(long int)                  {return 0;}
        virtual std::size_t getSizeValue(long long int)             {return 0;}
        virtual std::size_t getSizeValue(unsigned short int)        {return 0;}
        virtual std::size_t getSizeValue(unsigned int)              {return 0;}
        virtual std::size_t getSizeValue(unsigned long int)         {return 0;}
        virtual std::size_t getSizeValue(unsigned long long int)    {return 0;}
        virtual std::size_t getSizeValue(float)                     {return 0;}
        virtual std::size_t getSizeValue(double)                    {return 0;}
        virtual std::size_t getSizeValue(long double)               {return 0;}
        virtual std::size_t getSizeValue(bool)                      {return 0;}
        virtual std::size_t getSizeValue(std::string_view const&)   {return 0;}
        virtual std::size_t getSizeRaw(std::size_t)                 {return 0;}

        bool write(char const* src, std::size_t size)
        {
            struct Write
            {
                char const* src;
                std::size_t size;
                Write(char const* src, std::size_t size): src(src), size(size) {}

                bool operator()(std::ostream* output)       {return static_cast<bool>(output->write(src, size));}
                bool operator()(StringOutput& output)       {return output.write(src, size);}
            };
            return std::visit(Write{src, size}, output);
        }
        bool write(std::string_view const& src)
        {
            return write(std::begin(src), src.size());
        }
        bool ok() const
        {
            struct OK
            {
                bool operator()(std::ostream* output)       {return !output->fail();}
                bool operator()(StringOutput const& output) {return output.isOk();}
            };
            return std::visit(OK{}, output);
        }
        void setFail()
        {
            struct SetFail
            {
                void operator()(std::ostream* output)       {output->setstate(std::ios::failbit);}
                void operator()(StringOutput& output)       {output.setFail();}
            };
            std::visit(SetFail{}, output);
        }
        template<typename T>
        void writeValue(T const& src)
        {
            struct WriteValue
            {
                T const& src;
                WriteValue(T const& src): src(src) {}
                void operator()(std::ostream* output)       {(*output) << src;}
                void operator()(StringOutput& output)       {output.writeValue(src);}
            };
            std::visit(WriteValue{src}, output);
        }

        template<typename T>
        SharedInfo<T> addShared(std::shared_ptr<T> const& shared)
        {
            std::intmax_t index = reinterpret_cast<std::intmax_t>(shared.get());
            void const*&  save = savedSharedPtr[index];
            if (save == nullptr)
            {
                save = &shared;
            }
            SharedInfo<T>   result;
            result.sharedPtrName = index;
            if (save == &shared) {
                result.data     = shared.get();
            }
            return result;
        }
    private:
        using DataOutputStream = std::variant<std::ostream*, StringOutput>;

        DataOutputStream   output;
        std::map<std::intmax_t, void const*>     savedSharedPtr;
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "PrinterInterface.source"
#endif

#endif
