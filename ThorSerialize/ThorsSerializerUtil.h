#ifndef THORSANVIL_SERIALIZER_THORSSERIALIZERUTIL_H
#define THORSANVIL_SERIALIZER_THORSSERIALIZERUTIL_H

#include "SerializeConfig.h"
#include "PolymorphicMarker.h"
#include "PrinterConfig.h"
#include "StringOutput.h"
#include "StringInput.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <type_traits>
#include <string>
#include <map>
#include <any>
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <algorithm>
#include <functional>
#include <optional>
#include <memory>
#include <variant>

namespace ThorsAnvil::Serialize
{

template<typename T>
struct SharedInfo
{
    std::intmax_t       sharedPtrName;
    std::optional<T*>   data;
};

/*
 * Defines the generic type that all serialization types can expand on
 */
enum class TraitType {Invalid, Parent, Value, Map, Array, Enum, Pointer, Reference, Custom_Depricated, Custom_Serialize};
enum class FormatType{Json, Yaml, Bson};

template<typename T, typename SFINE = void>
class Traits;

template <typename, typename = void>
struct HasParent: std::false_type
{};

template <class T>
struct HasParent<T, std::enable_if_t<(sizeof(typename Traits<std::remove_cv_t<T>>::Parent) >= 0)>>: std::true_type
{};

/*
 * A class for holding multiple header types.
 * Multiple enheritance is a rare case but it does happen (has been requested).
 * Because we are using macros I can allow a comma seporated list of parents so we have to
 * group the types into a single unique type "Parents". Then we will specialize the parsing
 * and printing code to handle this as a special case.
 */
template<typename... P>
struct Parents: public std::tuple<P...> {};

/*
 * To help the macros check the parent type we need to extract the type.
 * There is a special case when we use "Parents" to get the first type
 */
template<typename T>
struct GetPrimaryParentType
{
    using type = T;
};
template<typename... Args>
struct GetPrimaryParentType<Parents<Args...>>
{
    using type = std::tuple_element_t<0, std::tuple<Args...>>;
};

struct IgnoreCallBack
{
    using AppendFunc = std::function<void(char const*, std::size_t)>;
    using ReadFunc   = std::function<void(std::istream&, char*, std::size_t)>;
    using IgnoreFunc = std::function<void(std::istream&, std::size_t)>;
    AppendFunc  append  = [](char const*, std::size_t){};
    ReadFunc    read    = [](std::istream& s, char* d, std::size_t size){s.read(d, size);};
    IgnoreFunc  ignore  = [](std::istream& s, std::size_t size)         {s.ignore(size);};
};

enum class ParseType   {Weak, Strict, Exact};
enum class ParserToken {Error, DocStart, DocEnd, MapStart, MapEnd, ArrayStart, ArrayEnd, Key, Value};
struct ParserConfig
{
    /*
     * These constructor are maintained for backwards compatability
     * Please use a the default constructor followed by the Set<Attribute>() methods
     */
    ParserConfig(IgnoreCallBack&& cb,
                 ParseType parseStrictness = ParseType::Weak,
                 std::string const& polymorphicMarker = Private::getDefaultPolymorphicMarker(),
                 bool catchExceptions = true)
        : parseStrictness(parseStrictness)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
        , ignoreCallBack(std::move(cb))
    {}
    ParserConfig(ParseType parseStrictness,
                 std::string const& polymorphicMarker = Private::getDefaultPolymorphicMarker(),
                 bool catchExceptions = true)
        : parseStrictness(parseStrictness)
        , polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    ParserConfig(std::string const& polymorphicMarker, bool catchExceptions = true)
        : polymorphicMarker(polymorphicMarker)
        , catchExceptions(catchExceptions)
    {}
    ParserConfig(bool catchExceptions)
        : catchExceptions(catchExceptions)
    {}
    ParserConfig(ParseType parseStrictness, bool catchExceptions)
        : parseStrictness(parseStrictness)
        , catchExceptions(catchExceptions)
    {}
    // Use this constructor.
    ParserConfig()
    {}
    ParserConfig& setParseStrictness(ParseType p_parseStrictness)               {parseStrictness = p_parseStrictness;       return *this;}
    ParserConfig& setPolymorphicMarker(std::string const& p_polymorphicMarker)  {polymorphicMarker = p_polymorphicMarker;   return *this;}
    ParserConfig& setCatchExceptions(bool p_catchExceptions)                    {catchExceptions = p_catchExceptions;       return *this;}
    ParserConfig& setUseOldSharedPtr()                                          {useOldSharedPtr = true;                    return *this;}
    ParserConfig& setValidateNoTrailingData()                                   {validateNoTrailingData = true;             return *this;}
    ParseType       parseStrictness         = ParseType::Weak;
    std::string     polymorphicMarker       = Private::getDefaultPolymorphicMarker();
    bool            catchExceptions         = true;
    long            parserInfo              = 0;
    IgnoreCallBack  ignoreCallBack;
    bool            useOldSharedPtr         = false;
    bool            validateNoTrailingData  = false;
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
        virtual FormatType formatType()                 = 0;
                ParserToken     getToken();
                void            pushBackToken(ParserToken token);
        virtual ParserToken     getNextToken()          = 0;
        virtual std::string     getKey()                = 0;

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

        virtual std::string getRawValue()                = 0;

        void    ignoreValue();

        bool            read(char* dst, std::size_t size)
        {
            struct Read
            {
                char*       dst;
                std::size_t size;
                Read(char* dst, std::size_t size):dst(dst),size(size){}
                bool operator()(std::istream* input)    {return static_cast<bool>(input->read(dst, size));}
                bool operator()(StringInput& input)     {return input.read(dst, size);}
            };
            return std::visit(Read{dst, size}, input);
        }
        bool            readTo(std::string& dst, char delim)
        {
            struct ReadTo
            {
                std::string&    dst;
                char            delim;
                ReadTo(std::string& dst, char delim):dst(dst),delim(delim){}
                bool operator()(std::istream* input)    {return static_cast<bool>(std::getline((*input), dst, delim));}
                bool operator()(StringInput& input)     {return input.readTo(dst, delim);}
            };
            return std::visit(ReadTo(dst, delim), input);
        }
        std::size_t     lastReadCount() const
        {
            struct LastReadCount
            {
                std::size_t operator()(std::istream const* input)    {return input->gcount();}
                std::size_t operator()(StringInput const& input)     {return input.getLastReadCount();}
            };
            return std::visit(LastReadCount{}, input);
        }
        std::streampos  getPos()
        {
            struct GetPos
            {
                std::streampos operator()(std::istream* input)    {return input->tellg();}
                std::streampos operator()(StringInput& input)     {return input.getPos();}
            };
            return std::visit(GetPos{}, input);
        }
        int             get()
        {
            struct Get
            {
                int operator()(std::istream* input)    {return input->get();}
                int operator()(StringInput& input)     {return input.get();}
            };
            return std::visit(Get{}, input);
        }
        void            ignore(std::size_t size)
        {
            struct Ignore
            {
                std::size_t size;
                Ignore(std::size_t size): size(size) {}
                void operator()(std::istream* input)    {input->ignore(size);}
                void operator()(StringInput& input)     {input.ignore(size);}
            };
            std::visit(Ignore{size}, input);
        }
        void            clear()
        {
            struct Clear
            {
                void operator()(std::istream* input)    {input->clear();}
                void operator()(StringInput& input)     {input.clear();}
            };
            std::visit(Clear{}, input);
        }
        void            unget()
        {
            struct Unget
            {
                void operator()(std::istream* input)    {input->unget();}
                void operator()(StringInput& input)     {input.unget();}
            };
            std::visit(Unget{}, input);
        }
        bool            ok() const
        {
            struct OK
            {
                bool operator()(std::istream const* input)    {return !input->fail();}
                bool operator()(StringInput const& input)     {return input.isOk();}
            };
            return std::visit(OK{}, input);
        }
        void setFail()
        {
            struct SetFail
            {
                void operator()(std::istream* input)    {input->setstate(std::ios::failbit);}
                void operator()(StringInput& input)     {input.setFail();}
            };
            std::visit(SetFail{}, input);
        }
        template<typename T>
        void readValue(T& value)
        {
            struct ReadValue
            {
                T& value;
                ReadValue(T& value) :value(value) {}
                void operator()(std::istream* input)    {(*input) >> value;}
                void operator()(StringInput& input)     {input.readValue(value);}
            };
            std::visit(ReadValue{value}, input);
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

    private:
        using DataInputStream = std::variant<std::istream*, StringInput>;

        DataInputStream input;
        ParserToken     pushBack;
        std::map<std::intmax_t, std::any>     savedSharedPtr;
        void    ignoreTheValue();
        void    ignoreTheMap();
        void    ignoreTheArray();

};

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
            , output(output)
        {}

                bool    preflightSize();
                void    reserveSize();
        virtual void    reset()     {}

        virtual ~PrinterInterface() {}
        virtual FormatType formatType()                 = 0;
        virtual void    openDoc()                       = 0;
        virtual void    closeDoc()                      = 0;
        virtual void    openMap(std::size_t size)       = 0;
        virtual void    closeMap()                      = 0;
        virtual void    openArray(std::size_t size)     = 0;
        virtual void    closeArray()                    = 0;

        virtual void    addKey(std::string const& key)  = 0;

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

        virtual void    addValue(std::string const&)    = 0;
        virtual void    addValue(std::string_view const&) = 0;

        virtual void    addRawValue(std::string const&) = 0;

        virtual void    addNull()                       = 0;

        void addValue(void*)        = delete;
        void addValue(void const*)  = delete;

        virtual bool        printerUsesSize()                       {return false;}
        virtual void        pushLevel(bool)                         {}
        virtual void        popLevel()                              {}
        virtual std::size_t getSizeMap(std::size_t /*count*/)       {return 0;}
        virtual std::size_t getSizeArray(std::size_t /*count*/)     {return 0;}
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
        virtual std::size_t getSizeValue(std::string const&)        {return 0;}
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
        bool write(std::string const& src)
        {
            return write(src.c_str(), src.size());
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

template<typename T, bool = HasParent<T>::value>
struct CalcSizeHelper
{
    std::size_t getPrintSize(PrinterInterface& printer, T const& /*object*/, std::size_t& count, std::size_t& memberSize)
    {
        return memberSize + printer.getSizeMap(count);
    }
};
template<typename T>
struct CalcSizeHelper<T, true>
{
    std::size_t getPrintSize(PrinterInterface& printer, T const& object, std::size_t& count, std::size_t& memberSize)
    {
        using Parent = typename Traits<std::remove_cv_t<T>>::Parent;
        return ThorsAnvil::Serialize::Traits<std::remove_cv_t<Parent>>::getPrintSizeTotal(printer, object, count, memberSize);
    }
};

/*
 * Basic Types:
 * ============
 *
 * Traits<short int>
 * Traits<int>
 * Traits<long int>
 * Traits<long long int>
 * Traits<unsigned short int>
 * Traits<unsigned int>
 * Traits<unsigned long int>
 * Traits<unsigned long long int>
 *
 * Traits<float>
 * Traits<double>
 * Traits<long double>
 *
 * Traits<bool>
 *
 * Traits<std::string>
 *
 * Partial Types:
 * ==============
 * Pointer Types
 *  Traits<T*>
 *
 * Special Thor Types
 * ==================
 *  Traits<Parents<>>
 *
 */

/*
 * Declare types that can be read/written directly by the parser/printer
 * as value types. Other compound types need to use the Serializer/Deserializer
 */

#define THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(TYPE)                \
template<>                                                          \
class Traits<TYPE>                                                  \
{                                                                   \
    public:                                                         \
        static constexpr TraitType type = TraitType::Value;         \
        static std::size_t getPrintSize(PrinterInterface& printer, TYPE const& value, bool) \
        {                                                           \
            return printer.getSizeValue(value);                     \
        }                                                           \
}

THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(char);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(unsigned char);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(signed char);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(short int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(long int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(long long int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(unsigned short int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(unsigned int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(unsigned long int);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(unsigned long long int);

THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(float);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(double);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(long double);

THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(bool);

THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(std::string);
THORSANVIL_TRAITS_FOR_BASIC_TYPE_VALUE(std::string_view);

#if 0
template<> class Traits<short int>              {public: THORSANVIL_TRAITS_FOR_VALUE(short int)};
template<> class Traits<int>                    {public: THORSANVIL_TRAITS_FOR_VALUE(int)};
template<> class Traits<long int>               {public: THORSANVIL_TRAITS_FOR_VALUE(long int)};
template<> class Traits<long long int>          {public: THORSANVIL_TRAITS_FOR_VALUE(long long int)};

template<> class Traits<unsigned short int>     {public: THORSANVIL_TRAITS_FOR_VALUE(unsigned short int)};
template<> class Traits<unsigned int>           {public: THORSANVIL_TRAITS_FOR_VALUE(unsigned int)};
template<> class Traits<unsigned long int>      {public: THORSANVIL_TRAITS_FOR_VALUE(unsigned long int)};
template<> class Traits<unsigned long long int> {public: THORSANVIL_TRAITS_FOR_VALUE(unsigned long long int)};

template<> class Traits<float>                  {public: THORSANVIL_TRAITS_FOR_VALUE(float)};
template<> class Traits<double>                 {public: THORSANVIL_TRAITS_FOR_VALUE(double)};
template<> class Traits<long double>            {public: THORSANVIL_TRAITS_FOR_VALUE(long double)};

template<> class Traits<bool>                   {public: THORSANVIL_TRAITS_FOR_VALUE(bool)};

template<> class Traits<std::string>            {public: THORSANVIL_TRAITS_FOR_VALUE(std::string const&)};
#endif

/*
 * A specialization for pointer objects.
 * They are a pointer type. When serialized a pointer will emit:
 *      If the pointer is nullptr:      A "null" object.
 *      Otherwise de-reference the pointer and emit like normal.
 *
 * If the de-referenced type has a Traits class then it will be normally
 * serialized. Otherwise there will be an error.
 */

template<typename T, bool test = std::is_abstract_v<T>>
struct TraitsAllocator
{
    static T* alloc() {return new T;}
};
template<typename T>
struct TraitsAllocator<T, true>
{
    static T* alloc() {return nullptr;}
};
template<typename T>
class Traits<T*>
{
    public:
        static constexpr TraitType type = TraitType::Pointer;
        static T*   alloc()         {return TraitsAllocator<T>::alloc();}
        static void release(T* p)   {delete p;}
        static std::size_t getPrintSize(PrinterInterface& printer, T* object, bool)
        {
            if (object)
            {
                return Traits<std::remove_cv_t<T>>::getPrintSize(printer, *object, true);
            }
            return printer.getSizeNull();
        }
};

/*
 * Specialization of Parents so we can handle them in normal streaming operations
 */
template<typename... Args>
class Traits<Parents<Args...>>
{
    public:
        static constexpr TraitType type = TraitType::Parent;

        template<typename ChildType, std::size_t Index>
        static std::size_t getPrintSizeTotalParent(PrinterInterface& printer, ChildType const& object, std::size_t& count, std::size_t& memberSize)
        {
            using Parent = std::tuple_element_t<Index, std::tuple<Args...>>;
            return Traits<std::remove_cv_t<Parent>>::getPrintSizeTotal(printer, static_cast<Parent const&>(object), count, memberSize);
        }

        template<typename ChildType, std::size_t... Seq>
        static std::size_t getPrintSizeTotalAllParents(PrinterInterface& printer, ChildType const& object, std::size_t& count, std::size_t& memberSize, std::index_sequence<Seq...> const&)
        {
            auto sums = {static_cast<std::size_t>(0), getPrintSizeTotalParent<ChildType, Seq>(printer, object, count, memberSize)...};
            return *(sums.end() - 1);
        }

        template<typename ChildType>
        static std::size_t getPrintSizeTotal(PrinterInterface& printer, ChildType const& object, std::size_t& count, std::size_t& memberSize)
        {
            return getPrintSizeTotalAllParents(printer, object, count, memberSize, std::make_index_sequence<sizeof...(Args)>());
        }
};

/* ----- Polymorphic Type check --- */
template<class T>
auto tryGetPolyMorphicPrintSize(PrinterInterface& printer, T const& object, bool poly, int) -> decltype(object.getPolyMorphicPrintSize(printer))
{
    // This uses a virtual method in the object to
    // call printPolyMorphicObject() the difference
    // will be the type of the template used as we will
    // get the type 'T' of the most derived type of
    // the actual runtime object.
    if (poly)
    {
        return object.getPolyMorphicPrintSize(printer);
    }
    return getNormalPrintSize(printer, object, 0, 0);
}

template<class T>
auto tryGetPolyMorphicPrintSize(PrinterInterface& printer, T const& object, bool, long) -> std::size_t
{
    // This version is called if the object foes not have a virtual
    // `printPolyMorphicObject()`. Thus you get a call to the current
    // object and thus we simply use `T` and we can simply print the
    // normal members.
    return getNormalPrintSize(printer, object, 0, 0);
}

template<typename T>
std::size_t getNormalPrintSize(PrinterInterface& printer, T const& object, std::size_t count, std::size_t memberSize)
{
    std::size_t result = Traits<std::remove_cv_t<T>>::getPrintSizeTotal(printer, object, count, memberSize);
    return result;
}

template<typename T, typename S = typename T::ThorsSerializerCustomObjectSize>
auto tryGetSizeFromSerializeType(PrinterInterface& printer, T const& value, int) -> decltype(S::size(value))
{
    std::size_t size = S::size(value);
    return printer.getSizeRaw(size);
}

template<typename T>
auto tryGetSizeFromSerializeType(PrinterInterface&, T const&, long) -> std::size_t
{
    ThorsLogAndThrowCritical("ThorsAnvil::Serialize",
                             "tryGetSizeFromSerializeType",
                             "BSON backward compatibility. See comments in function.");
    // This function is needed for backward compatibility to make things compile without
    // requiring user code to be changed.
    //
    // It will only ever be used if you are using bsonExporter()
    // Since this is new code it is not subject to backward compatibility so that is OK.
    //
    // If you want to make your code work with the BSON serialization and you have used
    // ThorsAnvil_MakeTraitCustom() to define an attribute the best way is to change to
    // the modern technique.
    //
    // For a quick hack (rather than an upgrade) you can force the code to use the above function.
    //
    //  Assuming you have the following declaration in your code:
    //      ThorsAnvil_MakeTraitCustom(MySerializableClass)
    //
    // To do this you need to add the following to this class:
    //
    //          using ThorsSerializerCustomObjectSize = MySizeClass;
    //
    // Then for the "MySizeClass" you must add the following static method declaration.
    //
    //          static std::size_t size(MySerializableClass const& value)
    //          {
    //              // this should return the size in bytes of `value` that will be placed on
    //              // the output stream by your method
    //              // std::ostream operator<<(std::ostream& str, MySerializableClass const& value);
    //              //
    //          }
    //
    // As noted this is a bit convoluted so we urge you to upgrade.
    // The upgraded method allows for different output depending on the Serialization method
    //      Json/Yaml/Bson
    //
    // Which will make the code more stable.
    //
    // Please look at test/ExceptionTest.h for a simple example.
}

inline void escapeString(PrinterInterface& printer, std::string const& value)
{
    using namespace std::string_literals;

    static auto isEscape = [](char c)
    {
        return (c >= 0x00 && c <= 0x1f) || c == '"' || c == '\\';
    };

    auto begin  = std::begin(value);
    auto end    = std::end(value);
    auto next = std::find_if(begin, end, isEscape);
    if (next == end)
    {
        printer.write(value);
    }
    else
    {
        while (next != end)
        {
            printer.write(&*begin, (next - begin));
            if (*next == '"')
            {
                printer.write(R"(\")"s);
                ++next;
            }
            else if (*next == '\\')
            {
                printer.write(R"(\\)"s);
                ++next;
            }
            else if (*next == 0x08)
            {
                printer.write(R"(\b)"s);
                ++next;
            }
            else if (*next == 0x0C)
            {
                printer.write(R"(\f)"s);
                ++next;
            }
            else if (*next == 0x0A)
            {
                printer.write(R"(\n)"s);
                ++next;
            }
            else if (*next == 0x0D)
            {
                printer.write(R"(\r)"s);
                ++next;
            }
            else if (*next == 0x09)
            {
                printer.write(R"(\t)"s);
                ++next;
            }
            else
            {
                std::stringstream ss;
                ss     << R"(\u)"
                       << std::setw(4)
                       << std::setfill('0')
                       << std::hex
                       << static_cast<unsigned int>(static_cast<unsigned char>(*next))
                       << std::dec;
                printer.write(ss.str());
                ++next;
            }
            /*
            else
            {
                110xxxxx

                stream << R("\u") << std::setw(4) << std::setfill('0') << std::hex << codepoint;
            }
            */
            begin = next;
            next = std::find_if(begin, end, isEscape);
        }
        printer.write(&*begin, (end - begin));
    }
}


}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ThorsSerializerUtil.source"
#endif

#endif
