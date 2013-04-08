
#ifndef THORSANVIL_SERIALIZE_JSON_SERIALIZE_H
#define THORSANVIL_SERIALIZE_JSON_SERIALIZE_H

/* Content:
 *
 * Used to define how a class is imported/exported via the Json Serialized
 *
 * Conventions:
 *      T:      The type of the base class
 *      I:      The type of the member
 *      MP:     The type of the member pointer
 *
 *      S:      Source type (std::ostream output) (ThorsAnvil::Json::ScannerSax input)
 *
 * Every type that is serializeable has a class JsonSerializeTraits.
 * For basic types (int/float etc) no definition is required and the default template version works
 * For compound types you need to define the type SerializeInfo.
 *      SerializeInfo:           It is a mpl vector of types.
 *                              Each type (in the vector) defines how to serialize
 *                              a member of the compound type.
 *
 * Boilerplate  code to create the appropriate types for SerializeInfo.
 *      #define THORSANVIL_SERIALIZE_JsonAttribute(member)
 *
 * Example:
 *      class MyClass
 *      {
 *          // STUFF
 *
 *          // If any members are private and need to be serialized then
 *          // JsonSerializeTraits<MyClass> must be a friend of the class so it can generate the appropriate code
 *
 *          friend class JsonSerializeTraits<MyClass>;
 *      };
 *
 *      namespace ThorsAnvil { namespace Serialize { namespace Json {
 *      template<>
 *      class JsonSerializeTraits<MyClass>
 *      {
 *          typedef MyClass                                                        LocalType;
 *          typedef void                                                           ParentType;
 *          static ThorsAnvil::Serialize::Json::JsonSerializeType const  type    = Map;
 *
 *          THORSANVIL_SERIALIZE_JsonAttribute(member1);
 *          THORSANVIL_SERIALIZE_JsonAttribute(member2);
 *          THORSANVIL_SERIALIZE_JsonAttribute(member3);
 *          typedef boost::mpl::vector<member1, member2, member3>   SerializeInfo;
 *      };
 *      }}}
 *
 * Now we can serialize/deserialize with:
 *      std::cout << jsonExport(myClassObj) << "\n";
 *      std::cin  >> jsonInport(myClassObj);
 *
 * Same for std::vector
 *      std::vector<MyClass>  vec;  // Will serialize any fundamental type or type with JsonSerializeTraits<> specialized for it
 *                                  // If JsonSerializeTraits<> is not specialized for a compound type you get a compile time
 *                                  // error
 *      std::cout << jsonExport(vec) << "\n";
 *      std::cin  >> jsonImport(vec);
 */
#include "Json/ScannerSax.h"
#include "Json/ParserShiftReduce.h"
#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <sstream>

#ifdef CLOUD_USE_CPLUSPLUS11

/* In C++11 use Vardc macros to help reduce boilerplate */

#define THOR_BUILD_SERIALIZE_CHAIN1(MACRO, MACRO_END, _1)                                  MACRO_END(_1)
#define THOR_BUILD_SERIALIZE_CHAIN2(MACRO, MACRO_END, _1, _2)                              MACRO(_1) MACRO_END(_2)
#define THOR_BUILD_SERIALIZE_CHAIN3(MACRO, MACRO_END, _1, _2, _3)                          MACRO(_1) MACRO(_2) MACRO_END(_3)
#define THOR_BUILD_SERIALIZE_CHAIN4(MACRO, MACRO_END, _1, _2, _3, _4)                      MACRO(_1) MACRO(_2) MACRO(_3) MACRO_END(_4)
#define THOR_BUILD_SERIALIZE_CHAIN5(MACRO, MACRO_END, _1, _2, _3, _4, _5)                  MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO_END(_5)
#define THOR_BUILD_SERIALIZE_CHAIN6(MACRO, MACRO_END, _1, _2, _3, _4, _5, _6)              MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO_END(_6)
#define THOR_BUILD_SERIALIZE_CHAIN7(MACRO, MACRO_END, _1, _2, _3, _4, _5, _6, _7)          MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO_END(_7)
#define THOR_BUILD_SERIALIZE_CHAIN8(MACRO, MACRO_END, _1, _2, _3, _4, _5, _6, _7, _8)      MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) MACRO_END(_8)
#define THOR_BUILD_SERIALIZE_CHAIN9(MACRO, MACRO_END, _1, _2, _3, _4, _5, _6, _7, _8, _9)  MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) MACRO(_8) MACRO_END(_9)

#define THOR_BUILD_SERIALIZE_CHAIN_INC(MACRO, MACRO_END, SIZE, ...) THOR_BUILD_SERIALIZE_CHAIN  ## SIZE (MACRO, MACRO_END, __VA_ARGS__)
#define THOR_BUILD_SERIALIZE_CHAIN(MACRO, SIZE, ...)     THOR_BUILD_SERIALIZE_CHAIN_INC(THOR_BUILD_SERIALIZE_ ## MACRO, THOR_BUILD_SERIALIZE_ ## MACRO ## _END, SIZE, __VA_ARGS__)

#define THOR_BUILD_SERIALIZE_COUNT_ARGS(...)             THOR_BUILD_SERIALIZE_COUNT_ARGS_ACT( __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1 )
#define THOR_BUILD_SERIALIZE_COUNT_ARGS_ACT(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...)   N

#define THOR_BUILD_SERIALIZE_TYPES(...)                  THOR_BUILD_SERIALIZE_CHAIN(TYPES_VAL, THOR_BUILD_SERIALIZE_COUNT_ARGS( __VA_ARGS__), __VA_ARGS__ )
#define THOR_BUILD_SERIALIZE_TYPES_VAL(val)              THORSANVIL_SERIALIZE_JsonAttribute(val);
#define THOR_BUILD_SERIALIZE_TYPES_VAL_END(val)          THORSANVIL_SERIALIZE_JsonAttribute(val);


#define THOR_BUILD_SERIALIZE_INFO(...)                   THOR_BUILD_SERIALIZE_CHAIN(INFO_VAL, THOR_BUILD_SERIALIZE_COUNT_ARGS( __VA_ARGS__) , __VA_ARGS__ )
#define THOR_BUILD_SERIALIZE_INFO_VAL(val)               val,
#define THOR_BUILD_SERIALIZE_INFO_VAL_END(val)           val


#define THOR_BUILD_SERIALIZE(parent, local, ...)        \
namespace ThorsAnvil {                                  \
namespace Serialize  {                                  \
namespace Json       {                                  \
    template<>                                          \
    struct JsonSerializeTraits<local>                   \
    {                                                   \
        typedef local               LocalType;          \
        typedef parent              ParentType;         \
        static JsonSerializeType const type = Map;      \
                                                        \
        THOR_BUILD_SERIALIZE_TYPES(__VA_ARGS__)         \
         typedef boost::mpl::vector<                    \
        THOR_BUILD_SERIALIZE_INFO(__VA_ARGS__)          \
        > SerializeInfo;                                \
    };                                                  \
}}} 

/*
 * Note: This code should be used outside all namespace.
 *       I considered forces people to manually put in the namspace
 *       But it looks ugly.
 *      
 *       We will let it be a learning curve;
 *       You will probably get it wrong and then come and read the source.
 *       look at the test.cpp file for an example of usage.
 */
#define JsonSerializeTraits_MAKE(parent,local, ...)     THOR_BUILD_SERIALIZE(parent, local, __VA_ARGS__)

#endif


/*
 * Helper Macros:
 * 
 * These are macros that will build some boilerplate types needed by the serialization code.
 *
 * THORSANVIL_SERIALIZE_JsonAttribute:          This is the main macro used.
 *                                              It identifies a class member that will be serialized
 *
 * THORSANVIL_SERIALIZE_JsonAttribute_1:        Used internally (should probably not be used by others).
 * THORSANVIL_SERIALIZE_JsonAttributeAccess:    If you want to run some code to as part of the serialization processes
 *                                              this macro allows you to specify a type that will be used during serialization.
 *                                              Examples will be provided in the documentaion.
 *
 * THORSANVIL_SERIALIZE_JsonGenericMapAttributeAccess:  A generic accessor can be used to generate multiple items.
 *                                                      When de-serializing the Json can be applied to multiple elements.
 *                                                      Used manly for container classes like std::map
 *THORSANVIL_SERIALIZE_JsonGenericArrAttributeAccess:   A generic accessor used by for arrays rather than maps (std::vector)
 *                                                      But otherwise identical to THORSANVIL_SERIALIZE_JsonGenericMapAttributeAccess
 */
#define THORSANVIL_SERIALIZE_JsonAttribute(member)                                                                           \
    typedef BOOST_TYPEOF(((LocalType*)01)->member)    JsonAttribute ## member ## Type;                                                  \
    THORSANVIL_SERIALIZE_JsonAttribute_1(member, JsonSerializeTraits<JsonAttribute ## member ## Type>)

#define THORSANVIL_SERIALIZE_JsonAttribute_1(member, SerTraits)                                                              \
    typedef BOOST_TYPEOF(&LocalType::member)            JsonAttribute ## member ## TypePtr;                                             \
    typedef JsonSerialElementAccessor<LocalType, JsonAttribute ## member ## TypePtr, SerTraits>  JsonAttribute ## member ## Accessor;   \
    struct member: JsonSerializeItem<LocalType, JsonAttribute ## member ## Accessor, std::string>                                       \
    {                                                                                                                                   \
        member()                                                                                                                        \
            : JsonSerializeItem<LocalType, JsonAttribute ## member ## Accessor, std::string>(#member, &LocalType::member)               \
        {}                                                                                                                              \
    }
#define THORSANVIL_SERIALIZE_JsonAttributeAccess(member, accessor)                                                           \
    struct member: JsonSerializeItem<LocalType, accessor, std::string>                                                                  \
    {                                                                                                                                   \
        member()                                                                                                                        \
            : JsonSerializeItem<LocalType, accessor, std::string>(#member, accessor())                                                  \
        {}                                                                                                                              \
    }
#define THORSANVIL_SERIALIZE_JsonGenericMapAttributeAccess(accessor)                                                         \
    struct genericAccessor: JsonSerializeItem<LocalType, accessor, std::string>                                                         \
    {                                                                                                                                   \
        genericAccessor()                                                                                                               \
            : JsonSerializeItem<LocalType, accessor, std::string>("\xFF", accessor())                                                   \
        {}                                                                                                                              \
    }
#define THORSANVIL_SERIALIZE_JsonGenericArrAttributeAccess(accessor)                                                         \
    struct genericAccessor: JsonSerializeItem<LocalType, accessor, int>                                                                 \
    {                                                                                                                                   \
        genericAccessor()                                                                                                               \
            : JsonSerializeItem<LocalType, accessor, int>(-1 , accessor())                                                              \
        {}                                                                                                                              \
    }



namespace ThorsAnvil
{
    namespace Serialize
    {
        /* External dependencies from the generic Serialization code */
        template<typename T, typename Parser>
        struct Importer;
                    
        template<typename T, typename Printer>
        struct Exporter;

        namespace Json
        {

/* Three basic element types:   Invalid (this obejct is not a top level JSON object)
 *                              Map     A JSON object   { [<string> : <value> [, <string> : <value>]*]? }
 *                              Array   A JSON array    [ [<value> [, <value>]*]? ]
 */
enum JsonSerializeType {Invalid, Map, Array}; 

/*
 * All objects that want to be serialized by this code must define their own specialization of this class.
 * The default version will cause compilation errors. Which hopefully will bring the reader here.
 */
template<typename T>
struct JsonSerializeTraits
{
	typedef void                  ParentType;
    static JsonSerializeType const  type    = Invalid;
    typedef T                     SerializeInfo;
};

// Forward declarations
template<typename T, typename A, typename RegisterKey>
struct JsonSerializeItem;

/* Class used by boost::mpl::for_each. Nothing special simple lamba's will replace them in the future */
/*
 *  T       The object Type
 *  S       The source type (parser or std::ostream)
 *
 * This is basically trying to templatize and remove the need for multiple action objects that
 * are called from mpl::for_each
 */
template<typename T, typename S>
struct MPLForEachActivateTrait;

template<typename T>
struct MPLForEachActivateTrait<T, std::ostream>
{
    typedef const T                 ObjectType;
    static int const                SerializeActionIndex    = 0;
};
template<typename T>
struct MPLForEachActivateTrait<T, ThorsAnvil::Json::ScannerSax>
{
    typedef T                       ObjectType;
    static int const                SerializeActionIndex    = 1;
};
template<typename T, typename S>
class MPLForEachActivateItem
{
    typedef          MPLForEachActivateTrait<T, S>  Traits;
    typedef typename Traits::ObjectType             ObjectType;

    S&              pump;
    ObjectType&     object;
    public:
        MPLForEachActivateItem(S& p, ObjectType& obj)
            : pump(p)
            , object(obj)
        {}
        // Depending on if the pump is a stream or a scanner
        // Calls      JsonSerialize::activate()
        // or         JsonDeSerialize::activate()
        template<typename SerializeItem>
        void operator()(SerializeItem const& item) const
        {
            typedef typename boost::mpl::at_c<typename SerializeItem::SerializeType, Traits::SerializeActionIndex>::type  SerializAction;
            SerializAction::activate(item, pump, object);
        }
};

/*
 * Objects of this type get stored in the
 *      JsonSerializeTraits::SerializeInfo
 *  This is what the user create with the macros above. The default A is JsonSerialElementAccessor
 *  But user can define their own action for complex objects This wrapper is merely a vehicle for
 *  calling the A methods in a controlled manner.
 *
 * Note: These classes are not designed to be used directly but via the macros:
 *          THORSANVIL_SERIALIZE_JsonAttribute
 *          THORSANVIL_SERIALIZE_JsonAttributeAccess
 *  See the notes by these macros for details
 */
template<typename T, typename A, typename RegisterKey, JsonSerializeType type = JsonSerializeTraits<T>::type>
struct JsonSerialize;


template<typename T, typename A, typename RegisterKey>
struct JsonSerialize<T, A, RegisterKey, Map>
{
    // Generic serialization of a JSON object
    static void activate(JsonSerializeItem<T, A, RegisterKey> const& item, std::ostream& stream, T const& src)
    {
        if (!item.first)
        {   stream << ',';
        }
        stream << '"' << item.memberName << '"' << ":";
        item.accessor.serialize(src, stream);
    }
};
template<typename C, typename A, typename RegisterKey>
struct JsonSerialize<C, A, RegisterKey, Array>
{
    // Generic serialization of a JSON array
    static void activate(JsonSerializeItem<C, A, RegisterKey> const& item, std::ostream& stream, C const& src)
    {
        if (!item.first)
        {   stream << ',';
        }
        item.accessor.serialize(src, stream);
    }
};

template<typename T, typename A,typename RegisterKey, JsonSerializeType type = JsonSerializeTraits<T>::type>
struct JsonDeSerialize;

template<typename T, typename A,typename RegisterKey>
struct JsonDeSerialize<T, A, RegisterKey, Map>
{
    static void activate(JsonSerializeItem<T, A, RegisterKey> const& item, ThorsAnvil::Json::ScannerSax& parser, T& dst)
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>    action(item.accessor.action(dst));
        parser.registerAction(item.memberName, SMART_OWNED_MOVE(action));
    }
};
template<typename T, typename A>
struct JsonDeSerialize<T, A, int, Array>
{
    static void activate(JsonSerializeItem<T, A, int> const& item, ThorsAnvil::Json::ScannerSax& parser, T& dst)
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>    action(item.accessor.action(dst));
        parser.registerActionOnAllArrItems(SMART_OWNED_MOVE(action));
    }
};

template<typename T, typename A>
struct JsonDeSerialize<T, A, std::string, Array>
{
    static void activate(JsonSerializeItem<T, A, std::string> const& item, ThorsAnvil::Json::ScannerSax& parser, T& dst)
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>    action(item.accessor.action(dst));
        parser.registerActionNext(SMART_OWNED_MOVE(action));
    }
};

/*
 * A type holder object that picks up the correct versions of JsonSerialize and JsonDeSerialize
 * Used by MPLForEachActivateItem to get the correct type
 */
template<typename T, typename A, typename RegisterKey>
struct JsonSerializeItem
{
    RegisterKey memberName;
    A           accessor;
    bool        first;

    JsonSerializeItem(RegisterKey const& name, A const& ac): memberName(name), accessor(ac), first(false) {}
    JsonSerializeItem& makeFirst() {first = true;return *this;}

    typedef    JsonSerialize<T,A,RegisterKey>       Serialize;
    typedef    JsonDeSerialize<T,A,RegisterKey>     DeSerialize;

    typedef boost::mpl::vector<Serialize, DeSerialize>      SerializeType;
};
/*
 * Importing
 * ============
 *
 * The JsonImportAction defines a class that is registered with a Json SAX parser
 * so that we can register callback actions for particular attributes.
 *
 * For fundamental types json is read directly into the value.
 * For compound types when the attribute is reached additional callbacks are registered
 * for each of the compound members that needs to be de-serialized (this is done recursively)
 * So we can de-serialize arbitrary json structures.
 */
template<typename I, bool EnableEnum = boost::is_enum<I>::value>
struct JsonImportPODValueExtractor
{
    I operator()(JsonValue const& value) const                  {return value.getValue<I>();}
};
template<typename I, bool EnableEnum = boost::is_enum<I>::value>
struct JsonExportPODValueExtractor
{
    void operator()(std::ostream& stream, I const& value) const {stream << value;}
};

template<typename I>
struct JsonImportEnumMappings;

template<typename I>
struct JsonImportPODValueExtractor<I, true>
{
    I operator()(JsonValue const& value) const
    {
        static const std::size_t    size    = sizeof(JsonImportEnumMappings<I>::stringMap)/sizeof(JsonImportEnumMappings<I>::stringMap[0]);
        std::string         sev     = value.getValue<std::string>();
        std::string const*  find    = std::find(&JsonImportEnumMappings<I>::stringMap[0], &JsonImportEnumMappings<I>::stringMap[size], sev);
        std::size_t         dist    = find - &JsonImportEnumMappings<I>::stringMap[0];
        if (dist >= size)
        {
            std::string         typeName(typeid(I).name());
            std::stringstream   msg;
            msg << typeName << ": Invalid Argument: " << sev;
            throw std::invalid_argument(msg.str());
        }
        return static_cast<I>(dist);
    }
};
template<typename I>
struct JsonExportPODValueExtractor<I, true>
{
    void operator()(std::ostream& stream, I const& value) const
    {
        stream << "\"" << JsonImportEnumMappings<I>::stringMap[value] << "\"";
    }
};

template<typename T, typename MemberToSerialize>
struct MemberScanner;

template<typename SerializeInfo, typename I, bool EnablePod = boost::mpl::or_<boost::is_fundamental<I>, boost::is_enum<I> >::value>
class JsonImportAction: public ThorsAnvil::Json::SaxAction
{
    I&              memberRef;
    public:
        JsonImportAction(I& mr)
            : memberRef(mr)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&){}
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const& value)
        {
            // Read fundamental type directly into the member
            JsonImportPODValueExtractor<I>     extractor;
            memberRef   = extractor(value);
        }
};
template<typename SerializeInfo, typename I>
class JsonImportAction<SerializeInfo, I, false>: public ThorsAnvil::Json::SaxAction
{
    I&              memberRef;
    public:
        JsonImportAction(I& mr)
            : memberRef(mr)
        {}

        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const&){}
        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& parser, ThorsAnvil::Json::Key const&)
        {
            MemberScanner<I, SerializeInfo>   scanner;
            scanner(parser,memberRef);
        }
};

/*
 * Need a function template to create the correct JsonImportAction()
 */
template<typename SerializeInfo, typename T, typename I>
ThorsAnvil::Json::SaxAction* new_JsonImportAction(T& dst, I T::* memberPtr)
{
    return new JsonImportAction<SerializeInfo, I>(dst.*memberPtr);
}


/* Default Serialization Traits:
 * Used by all types without their own specific serialization traits.
 */
template<JsonSerializeType>
struct JsonSerializeBrace
{
    static char braces[];
};

/*
 * The MemberScanner is used to register callbacks that will read sub-members from the json object
 */
template<typename T, bool hasParent = boost::is_void<typename JsonSerializeTraits<T>::ParentType>::value>
struct AppendParentInfo
{
    typedef typename JsonSerializeTraits<T>::SerializeInfo                                          MemberToSerialize;
    typedef typename JsonSerializeTraits<T>::ParentType                                             ParentType;
    typedef typename AppendParentInfo<ParentType>::type                                             ParentMembers;
    typedef typename boost::mpl::end<MemberToSerialize>::type                                       memEnd;
    typedef typename boost::mpl::insert_range<MemberToSerialize, memEnd, ParentMembers>::type       type;
};

template<typename T>
struct AppendParentInfo<T, true>
{
    typedef typename JsonSerializeTraits<T>::SerializeInfo                                          type;
};
template<typename T, typename MemberToSerialize = typename JsonSerializeTraits<T>::SerializeInfo>
struct MemberScanner
{
    void operator()(ThorsAnvil::Json::ScannerSax& scanner, T& destination)
    {
        typedef typename AppendParentInfo<T>::type  AllMembers;
        boost::mpl::for_each<AllMembers>(MPLForEachActivateItem<T, ThorsAnvil::Json::ScannerSax>(scanner, destination));
    }
};
template<typename T>
struct MemberScanner<T, T>
{
    // A normal type with no SerializeInfo has no members thus no need to register callbacks.
    void operator()(ThorsAnvil::Json::ScannerSax& scanner, T& destination)
    {}
};
template<typename T>
struct MemberScanner<T, void>
{
    // A normal type with no SerializeInfo has no members thus no need to register callbacks.
    void operator()(ThorsAnvil::Json::ScannerSax& scanner, T& destination)
    {}
};

/*
 * The MemberPrinter prints each member of an object.
 */
template<typename T, typename MemberToSerialize = typename JsonSerializeTraits<T>::SerializeInfo>
struct MemberPrinter
{
    void operator()(std::ostream& stream, T const& source)
    {
        typedef typename AppendParentInfo<T>::type  AllMembers;
        typedef typename boost::mpl::at<AllMembers, boost::integral_constant<int,0> >::type  FirstType;
        typedef typename boost::mpl::pop_front<AllMembers>::type                             AllButFirstType;

        MPLForEachActivateItem<T, std::ostream>     itemPrinter(stream, source);

        // Print the first member (Call makeFirst() means no comma is printed)
        itemPrinter(FirstType().makeFirst());

        // For each of the other members use a loop a proceed each object with a comma
        boost::mpl::for_each<AllButFirstType>(itemPrinter);
    }
};
template<typename T>
struct MemberPrinter<T, T>
{
    // A normal object just prints itself.
    void operator()(std::ostream& stream, T const& source)
    {
        JsonExportPODValueExtractor<T> extractor;
        extractor(stream, source);
    }
};
template<typename T>
struct MemberPrinter<T, void>
{
    void operator()(std::ostream& stream, T const& source)
    {}
};


struct JsonSerializer
{
    template<typename T, JsonSerializeType base = JsonSerializeTraits<T>::type>
    struct Parser: ThorsAnvil::Json::ScannerSax
    {
        typedef boost::mpl::bool_<base != Invalid>      Parsable;
        Parser(T& dst)
            : destination(dst)
        {
            MemberScanner<T>    memberScanner;
            memberScanner(*this, destination);
        }

        void parse(std::istream& stream)
        {
            ScannerSax::parse<yy::ParserShiftReduce>(stream);
        }

        T&  destination;
    };

    template<typename T ,JsonSerializeType base = JsonSerializeTraits<T>::type>
    struct Printer
    {
        typedef boost::mpl::bool_<base != Invalid>      Printable;
        Printer(T const& src)
            : source(src)
        {}

        void print(std::ostream& stream)
        {
            stream << JsonSerializeBrace<base>::braces[0];

            MemberPrinter<T>    memberPrinter;
            memberPrinter(stream, source);

            stream << JsonSerializeBrace<base>::braces[1];
        }

        T const&    source;
    };

};

/*
 * Default accessors for fundamental types std::string
 * The serialize()  Recursively calls jsonInternalExport() on the member to serialize the member.
 *
 * The action()     Creates a JsonImportAction() that is registered with the SAX parser that just reads the
 *                  Item directly into the object. If the object is a compound type it uses the SerializeInfo
 *                  to register subsequent actions recursively so we always read directly into an object
 *                  not a copy.
 */
template<typename T, typename MP, typename SerTraits>
class JsonSerialElementAccessor
{
    MP          memberPtr;
    public:
    JsonSerialElementAccessor(MP mp): memberPtr(mp)    {}
    void serialize(T const& src, std::ostream& stream) const
    {
        stream << jsonInternalExport(src.*memberPtr);
    }
    SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>      action(T& dst) const
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>  action(new_JsonImportAction<typename SerTraits::SerializeInfo>(dst, memberPtr));
        return action;
    }
};

/* Helper functions */
template<typename T>
Importer<T, typename JsonSerializer::template Parser<T> >   jsonInternalImport(T& object)
{
    return Importer<T, typename JsonSerializer::template Parser<T> >(object);
}

template<typename T>
Exporter<T, typename JsonSerializer::template Printer<T> >  jsonInternalExport(T const& object)
{
    return Exporter<T, typename JsonSerializer::template Printer<T> >(object);
}




        }
    }
}


#endif

