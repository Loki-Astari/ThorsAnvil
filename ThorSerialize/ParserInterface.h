#ifndef THORSANVIL_SERIALIZER_PARSER_INTERFACE_H
#define THORSANVIL_SERIALIZER_PARSER_INTERFACE_H

#include "SerializeConfig.h"
#include "ThorsSerializerUtilTypes.h"
#include "ParserConfig.h"
#include "StringInput.h"
#include <cstddef>
#include <variant>
#include <string>
#include <string_view>
#include <iostream>
#include <map>
#include <any>
#include <memory>
#include <concepts>
#include <locale>

namespace ThorsAnvil::Serialize
{

struct ReadValue
{
    template<typename I>
    void checkZero(I& stream) const
    {
        int peek = stream.peek();
        if (peek == '0')
        {
            stream.get();
            peek = stream.peek();
            if (peek >= '1' && peek <= '9') {
                throw std::runtime_error("Nubers can't start with leading zero");
            }
            else {
                stream.unget();
            }
        }
    }
    template<typename T, typename I>
    requires std::integral<T>
    bool validateResult(bool ok, I& stream) const
    {
        char next = stream.peek();
        return ok && next != '.' && next != 'e' && next != 'E';
    }
    template<typename T, typename I>
    requires std::floating_point<T>
    bool validateResult(bool ok, I&) const
    {
        return ok;
    }
    template<typename X>
    bool readNumber(std::istream& input, X& value) const
    requires ((std::integral<X> && std::is_unsigned_v<X>) || std::floating_point<X>)
    {
        std::locale         loc;
        std::ios::iostate   state{std::ios::goodbit};
        auto                before = input.tellg();
        std::use_facet<std::num_get<char>>(loc).get(input, std::istreambuf_iterator<char>(), input, state, value);
        return before != input.tellg();
    }
    template<typename X>
    bool readNumber(std::istream& input, X& value) const
    requires (std::integral<X> && !std::is_unsigned_v<X>)
    {
        bool neg = input.peek() == '-';
        if (neg) {
            input.get();
        }
        std::make_unsigned_t<X>     tmp;
        bool ok = readNumber(input, tmp);
        value = neg ? -tmp : tmp;
        return ok;
    }
    template<typename T>
    bool operator()(std::istream* input, T* value) const
    {
        checkZero(*input);
        bool ok = readNumber(*input, *value);
        return validateResult<T>(ok, *input);
    }
    template<typename T>
    bool operator()(StringInput& input, T* value) const
    {
        checkZero(input);
        bool ok = input.readValue(*value);
        return validateResult<T>(ok, input);
    }
};

struct PeekNextNonSpaceValue
{
    template<typename I>
    void checkAction(I& input, char value) const
    {
        switch (value)
        {
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
            case ':':
                break;
            default:
                input.unget();
        }
    }
    int operator()(std::istream* input) const
    {
        char value = -1;
        if ((*input) >> value) {
            checkAction(*input, value);
        }
        return value;
    }
    int operator()(StringInput& input) const
    {
        char value = -1;
        if (input.readValue(value)) {
            checkAction(input, value);
        }
        return value;
    }
};

class ParserInterface
{
    public:
        ParserConfig const config;

        ParserInterface(std::string_view const& str, ParserConfig  config = ParserConfig{})
            : config(config)
            , input(str)
            , pushBack(ParserToken::Error)
        {}
        ParserInterface(std::istream& stream, ParserConfig  config = ParserConfig{})
            : config(config)
            , input(&stream)
            , pushBack(ParserToken::Error)
        {}
        virtual ~ParserInterface() {}
        virtual FormatType       formatType()            = 0;
                ParserToken      getToken();
                void             pushBackToken(ParserToken token);
        virtual ParserToken      getNextToken()          = 0;
        virtual std::string_view getKey()                = 0;

        virtual void    ignoreDataValue()               {}
        virtual void    ignoreDataMap(bool)             {}
        virtual void    ignoreDataArray(bool)           {}

        virtual void    getValue(short int&)             = 0;
        virtual void    getValue(int&)                   = 0;
        virtual void    getValue(long int&)              = 0;
        virtual void    getValue(long long int&)         = 0;

        virtual void    getValue(unsigned short int&)    = 0;
        virtual void    getValue(unsigned int&)          = 0;
        virtual void    getValue(unsigned long int&)     = 0;
        virtual void    getValue(unsigned long long int&)= 0;

        virtual void    getValue(float&)                 = 0;
        virtual void    getValue(double&)                = 0;
        virtual void    getValue(long double&)           = 0;

        virtual void    getValue(bool&)                  = 0;

        virtual void    getValue(std::string&)           = 0;

        virtual bool    isValueNull()                    = 0;

        virtual std::string_view getRawValue()           = 0;

        void    ignoreValue();

        bool            read(char* dst, std::size_t size)
        {
            struct Read
            {
                bool operator()(std::istream* input, char* dst, std::size_t size)    const {return static_cast<bool>(input->read(dst, size));}
                bool operator()(StringInput& input, char* dst, std::size_t size)     const {return input.read(dst, size);}
            };
            using Dst   = std::variant<char*>;
            using Size  = std::variant<std::size_t>;
            return std::visit(Read{}, input, Dst{dst}, Size{size});
        }
        bool            readTo(std::string& dst, char delim)
        {
            struct ReadTo
            {
                bool operator()(std::istream* input, std::string* dst, char delim)    const {return static_cast<bool>(std::getline((*input), *dst, delim));}
                bool operator()(StringInput& input, std::string* dst, char delim)     const {dst->clear();return input.readTo(*dst, delim);}
            };
            using Dst   = std::variant<std::string*>;
            using Delim = std::variant<char>;
            return std::visit(ReadTo{}, input, Dst{&dst}, Delim{delim});
        }
        std::size_t     lastReadCount() const
        {
            struct LastReadCount
            {
                std::size_t operator()(std::istream const* input)    const {return input->gcount();}
                std::size_t operator()(StringInput const& input)     const {return input.getLastReadCount();}
            };
            return std::visit(LastReadCount{}, input);
        }
        std::streampos  tellg()
        {
            struct GetPos
            {
                std::streampos operator()(std::istream* input)    const {return input->tellg();}
                std::streampos operator()(StringInput& input)     const {return input.tellg();}
            };
            return std::visit(GetPos{}, input);
        }
        int             get()
        {
            struct Get
            {
                int operator()(std::istream* input)    const {return input->get();}
                int operator()(StringInput& input)     const {return input.get();}
            };
            return std::visit(Get{}, input);
        }
        int             peek()
        {
            struct Peek
            {
                int operator()(std::istream* input)    const {return input->peek();}
                int operator()(StringInput& input)     const {return input.peek();}
            };
            return std::visit(Peek{}, input);
        }
        void            ignore(std::size_t size)
        {
            struct Ignore
            {
                void operator()(std::istream* input, std::size_t size)    const {input->ignore(size);}
                void operator()(StringInput& input, std::size_t size)     const {input.ignore(size);}
            };
            using Size = std::variant<std::size_t>;
            std::visit(Ignore{}, input, Size{size});
        }
        void            clear()
        {
            struct Clear
            {
                void operator()(std::istream* input)    const {input->clear();}
                void operator()(StringInput& input)     const {input.clear();}
            };
            std::visit(Clear{}, input);
        }
        void            unget()
        {
            struct Unget
            {
                void operator()(std::istream* input)    const {input->unget();}
                void operator()(StringInput& input)     const {input.unget();}
            };
            std::visit(Unget{}, input);
        }
        bool            ok() const
        {
            struct OK
            {
                bool operator()(std::istream const* input)    const {return !input->fail();}
                bool operator()(StringInput const& input)     const {return input.isOk();}
            };
            return std::visit(OK{}, input);
        }
        void setFail()
        {
            struct SetFail
            {
                void operator()(std::istream* input)    const {input->setstate(std::ios::failbit);}
                void operator()(StringInput& input)     const {input.setFail();}
            };
            std::visit(SetFail{}, input);
        }
        template<typename T>
        requires std::integral<T>
        bool readValue(T& value)
        {
            using Value = std::variant<T*>;
            return std::visit(ReadValue{}, input, Value{&value});
        }
        template<typename T>
        requires std::floating_point<T>
        bool readValue(T& value)
        {
            using Value = std::variant<T*>;
            return std::visit(ReadValue{}, input, Value{&value});
        }
        bool readValue(char& value)
        {
            struct ReadValue
            {
                bool operator()(std::istream* input, char* value)    const {return static_cast<bool>((*input) >> (*value));}
                bool operator()(StringInput& input, char* value)     const {return input.readValue(*value);}
            };
            using Value = std::variant<char*>;
            return std::visit(ReadValue{}, input, Value{&value});
        }

        int peekNextNonSpaceValue()
        {
            return std::visit(PeekNextNonSpaceValue{}, input);
        }
        template<typename T>
        void getShared(SharedInfo<T> const& info, std::shared_ptr<T>& object)
        {
            std::intmax_t index = info.sharedPtrName;
            if (info.data.has_value())
            {
                object.reset(info.data.value());
                savedSharedPtr[index] = object;
                return;
            }
            std::shared_ptr<T>  sharedPtr = std::any_cast<std::shared_ptr<T>>(savedSharedPtr[index]);
            object = sharedPtr;
        }
        std::string identifyDynamicType()
        {
            return config.identifyDynamcClass(input);
        }

    private:
        using DataInputStream = std::variant<std::istream*, StringInput>;

        DataInputStream input;
        ParserToken     pushBack;
        std::map<std::intmax_t, std::any>     savedSharedPtr;
        void    ignoreTheValue();
        void    ignoreTheMap();
        void    ignoreTheArray();

};


}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ParserInterface.source"
#endif

#endif
