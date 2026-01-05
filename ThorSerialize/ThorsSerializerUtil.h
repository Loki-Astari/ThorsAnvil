#ifndef THORSANVIL_SERIALIZER_THORSSERIALIZERUTIL_H
#define THORSANVIL_SERIALIZER_THORSSERIALIZERUTIL_H

#include "SerializeConfig.h"
#include "ThorsSerializerUtilTypes.h"
#include "PolymorphicMarker.h"
#include "PrinterConfig.h"
#include "ParserConfig.h"
#include "PrinterInterface.h"
#include "ParserInterface.h"
#include "StringOutput.h"
#include "StringInput.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <stdexcept>
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

/*
 * Defines the generic type that all serialization types can expand on
 */
enum class TraitType {Invalid, Parent, Value, Map, Array, Enum, Pointer, Reference, Variant, Custom_Depricated, Custom_Serialize, AnyBlock};

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
    std::size_t count = 0;
    std::size_t memberSize = 0;

    if constexpr (Private::CheckForPoly<T>::value)
    {
        ++count;
        memberSize = printer.getSizeMember(ThorsAnvil::Serialize::Private::getPolymorphicMarker<T>(printer.config.polymorphicMarker))
                   + printer.getSizeValue(std::string(T::polyMorphicSerializerName()));
    }
    return getNormalPrintSize(printer, object, count, memberSize);
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

class DepricatedIssue: public std::runtime_error
{
    public:
        using std::runtime_error::runtime_error;
};

template<typename T>
auto tryGetSizeFromSerializeType(PrinterInterface&, T const&, long) -> std::size_t
{
    ThorsLogAndThrowError(DepricatedIssue,
                          "ThorsAnvil::Serialize",
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

inline void escapeString(PrinterInterface& printer, std::string_view const& value)
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

#endif
