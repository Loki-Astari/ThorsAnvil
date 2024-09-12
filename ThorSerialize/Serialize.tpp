#ifndef THORSANVIL_SERIALIZE_SERIALIZE_TPP
#define THORSANVIL_SERIALIZE_SERIALIZE_TPP

#include "SerUtil.h"
#include "ThorsSerializerUtil.h"
#include "ThorsIOUtil/Utility.h"
#include "ThorsLogging/ThorsLogging.h"
#include <algorithm>
#include <sstream>
#include <type_traits>
#include <map>
#include <unordered_map>


namespace ThorsAnvil::Serialize
{

/* ------------ ApplyActionToParent ------------------------- */
template<typename P, typename T, typename I>
class ApplyActionToAllParent
{
    public:
        void printParentMembers(Serializer& serializer, T const& object)
        {
            serializer.printObjectMembers(static_cast<P const&>(object));
        }
        bool scanParentMember(DeSerializer& deSerializer, I const& key, T& object)
        {
            return deSerializer.scanObjectMembers(key, static_cast<P&>(object));
        }
};
template<typename... Args, typename T, typename I>
class ApplyActionToAllParent<Parents<Args...>, T, I>
{
    public:
        void printParentMembers(Serializer& serializer, T const& object)
        {
            (serializer.printObjectMembers(static_cast<Args const&>(object)), ...);
        }
        bool scanParentMember(DeSerializer& deSerializer, I const& key, T& object)
        {
            /*
             * See if the key is valid in one parent
             */
            bool result = (deSerializer.scanObjectMembers(key, static_cast<Args&>(object)) || ...);
            return result;
        }
};

template<typename T, typename I>
class ApplyActionToParent<TraitType::Parent, T, I>
{
    ApplyActionToAllParent<typename Traits<std::remove_cv_t<T>>::Parent, T, I>  parentAction;
    public:
        void printParentMembers(Serializer& serializer, T const& object)
        {
            parentAction.printParentMembers(serializer, object);
        }
        bool scanParentMember(DeSerializer& deSerializer, I const& key, T& object)
        {
            return parentAction.scanParentMember(deSerializer, key, object);
        }
};
/* ------------------- HeedAllValues ---------------------------- */
template<typename T>
struct HeedAllValues;

template<typename T>
typename std::enable_if_t<! HasParent<T>::value>
heedAllParentMembers(std::set<std::string> const& /*membersound*/)
{}

template<typename T>
std::enable_if_t<HasParent<T>::value>
heedAllParentMembers(std::set<std::string> const& membersFound)
{
    HeedAllValues<typename Traits<std::remove_cv_t<T>>::Parent>   heedParent;
    heedParent(membersFound);
}

template<typename T>
struct HeedAllValues
{
    template<typename X>
    int checkAMember(std::set<std::string> const& membersFound, std::pair<char const*, X> const& member)
    {
        if (membersFound.find(member.first) == std::end(membersFound))
        {
            ThorsLogAndThrow("ThorsAnvil::Serialize::HeedAllValues",
                             "checkAMember"
                             "Did not fine: ", member.first);
        }
        return 0;
    }

    template<typename Tuple, std::size_t... Index>
    void checkEachMember(std::set<std::string> const& membersFound, Tuple const& tuple, std::index_sequence<Index...> const&)
    {
        (checkAMember(membersFound, std::get<Index>(tuple)), ...);
        heedAllParentMembers<T>(membersFound);
    }

    template<typename... Args>
    void checkMemberFound(std::set<std::string> const& membersFound, std::tuple<Args...> const& args)
    {
        checkEachMember(membersFound, args, std::index_sequence_for<Args...>{});
    }

    void operator()(std::set<std::string> const& membersFound)
    {
        checkMemberFound(membersFound, Traits<std::remove_cv_t<T>>::getMembers());
    }
};
template<typename... P>
struct HeedAllValues<Parents<P...>>
{
    template<typename ParentTupple, std::size_t... Index>
    void checkEachParent(ParentTupple& parentsToHeed, std::set<std::string> const& membersFound, std::index_sequence<Index...> const&)
    {
        (std::get<Index>(parentsToHeed)(membersFound), ...);
    }
    void operator()(std::set<std::string> const& membersFound)
    {
        std::tuple<HeedAllValues<P>...>     parentsToHeed;
        checkEachParent(parentsToHeed, membersFound, std::index_sequence_for<P...>{});
    }
};

template<typename K, typename V>
struct HeedAllValues<std::map<K, V>>
{
    void operator()(std::set<std::string> const& /*members*/) {}
};
template<typename K, typename V>
struct HeedAllValues<std::multimap<K, V>>
{
    void operator()(std::set<std::string> const& /*members*/) {}
};
template<typename K, typename V>
struct HeedAllValues<std::unordered_map<K, V>>
{
    void operator()(std::set<std::string> const& /*members*/) {}
};
template<typename K, typename V>
struct HeedAllValues<std::unordered_multimap<K, V>>
{
    void operator()(std::set<std::string> const& /*members*/) {}
};

/* ------------ DeSerializationForBlock ------------------------- */

/*
 * The default Block is a mapping of "Map" to "Object"
 * We expect an OpenMap followed by a set of Key/Value pairs followed by CloseMap
 */
template<TraitType traitType, typename T>
class DeSerializationForBlock
{
    static_assert(
        traitType != TraitType::Invalid,
        "Invalid Serialize TraitType. This usually means you have not define ThorsAnvil::Serialize::Traits<Your Type>"
    );
    DeSerializer&    parent;
    ParserInterface& parser;
    std::string      key;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {
            ParserToken    tokenType = parser.getToken();

            if (tokenType != ParserToken::MapStart)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Map>",
                                 "DeSerializationForBlock",
                                 "Invalid Object Start");
            }
        }

        void scanObject(T& object)
        {
            std::set<std::string>     memberFound;
            while (hasMoreValue())
            {
                if (!parent.scanObjectMembers(key, object))
                {
                    parser.ignoreValue();
                }
                else
                {
                    if (parser.config.parseStrictness == ParseType::Exact) {
                        memberFound.insert(key);
                    }
                }
            }
            if (parser.config.parseStrictness == ParseType::Exact)
            {
                HeedAllValues<T>    check;
                check(memberFound);
            }
        }
        bool hasMoreValue()
        {
            ParserToken    tokenType = parser.getToken();
            bool           result    = tokenType != ParserToken::MapEnd;
            if (result)
            {
                if (tokenType != ParserToken::Key)
                {
                    ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Map>",
                                     "hasMoreValue",
                                     "Expecting key token");
                }
                key = parser.getKey();
            }

            return result;
        }
};

/*
 * Specialization for Value.
 * This is only used at the top level.
 * There is no open or close. Just a single value is expected.
 */
template<typename T>
class DeSerializationForBlock<TraitType::Value, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            ParserToken    tokenType = parser.getToken();
            if (tokenType != ParserToken::Value)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Value>",
                                 "DeSerializationForBlock",
                                 "Invalid Object");
            }
            parser.getValue(object);
        }
};
template<typename T>
class [[deprecated("This is caused by suing the ThorsAnvil_MakeTraitCustom macro. This is a bit hacky as it simply drops data into the stream. Please upgrade to ThorsAnvil_SelfSerialize as this allows custom behavior depending on the underlying format Json/Yaml/Bson")]]
DeSerializationForBlock<TraitType::Custom_Depricated, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            ParserToken    tokenType = parser.getToken();
            if (tokenType != ParserToken::Value)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Serialize>",
                                 "DeSerializationForBlock",
                                 "Invalid Object");
            }
            std::stringstream valueStream(parser.getRawValue());
            valueStream >> object;
        }
};
template<typename T>
class DeSerializationForBlock<TraitType::Custom_Serialize, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            ParserToken    tokenType = parser.getToken();
            if (tokenType != ParserToken::Value)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Value>",
                                 "DeSerializationForBlock",
                                 "Invalid Object");
            }
            using SerializingType = typename Traits<std::remove_cv_t<T>>::SerializingType;
            SerializingType info;
            info.readCustom(parser, object);
        }
};


/* ------------ tryParsePolyMorphicObject Serializer ------------------------- */
template<typename T>
struct ConvertPointer
{
    static T* assign(T* result)
    {
        return result;
    }
};
template<typename T>
struct ConvertPointer<std::unique_ptr<T>>
{
    static std::unique_ptr<T> assign(T* result)
    {
        return std::unique_ptr<T>{result};
    }
};

template<typename T>
struct ConvertPointer<std::shared_ptr<T>>
{
    static std::shared_ptr<T> assign(T* result)
    {
        return std::shared_ptr<T>{result};
    }
};

template<class T>
auto tryParsePolyMorphicObject(DeSerializer& parent, ParserInterface& parser, T& object, int) -> decltype(object->parsePolyMorphicObject(parent, parser), void())
{
    ParserToken    tokenType;
    tokenType = parser.getToken();
    if (tokenType != ParserToken::MapStart)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize",
                         "tryParsePolyMorphicObject",
                         "Invalid Object. Expecting MapStart");
    }

    tokenType = parser.getToken();
    if (tokenType != ParserToken::Key)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize",
                         "tryParsePolyMorphicObject",
                         "Invalid Object. Expecting Key");
    }


    std::string keyValue;
    if (parser.getKey() != parser.config.polymorphicMarker)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize",
                         "tryParsePolyMorphicObject",
                         "Invalid PolyMorphic Object. Expecting Key Name ", parser.config.polymorphicMarker);
    }

    tokenType = parser.getToken();
    if (tokenType != ParserToken::Value)
    {
        ThorsLogAndThrow("ThorsAnvil::Serialize",
                         "tryParsePolyMorphicObject",
                         "Invalid Object. Expecting Value");
    }

    std::string className;
    parser.getValue(className);

    using BaseType  = std::remove_pointer_t<T>;
    using AllocType = typename GetAllocationType<BaseType>::AllocType;
    object = ConvertPointer<BaseType>::assign(PolyMorphicRegistry::getNamedTypeConvertedTo<AllocType>(className));

    // This uses a virtual method in the object to
    // call parsePolyMorphicObject() the difference
    // will be the type of the template used as we will
    // get the type 'T' of the most derived type of
    // the actual runtime object.
    //
    // To install this virtual method use the macro
    // ThorsAnvil_PolyMorphicSerializer  See Traits.h for details.
    parser.pushBackToken(ParserToken::MapStart);
    object->parsePolyMorphicObject(parent, parser);
}
template<class T>
auto tryParsePolyMorphicObject(DeSerializer& parent, ParserInterface& parser, T& object, long) -> void
{
    using TraitPoint = Traits<std::remove_cv_t<T>>;
    object = TraitPoint::alloc();

    parsePolyMorphicObject(parent, parser, *object);
}
/* ------------ PolyMorphic Serializer ------------------------- */
template<typename T>
void parsePolyMorphicObject(DeSerializer& parent, ParserInterface& parser, T& object)
{
    using TraitBase = Traits<std::remove_cv_t<T>>;
    DeSerializationForBlock<TraitBase::type, T>   pointerDeSerializer(parent, parser);
    pointerDeSerializer.scanObject(object);
}

template<typename T>
class DeSerializationForBlock<TraitType::Pointer, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            Traits<std::remove_cv_t<T>>::release(object);

            ParserToken    tokenType = parser.getToken();
            if (parser.isValueNull())
            {
                parser.ignoreDataValue();
                object = nullptr;
                return;
            }

            parser.pushBackToken(tokenType);

            tryParsePolyMorphicObject(parent, parser, object, 0);
        }
};
template<typename T>
class DeSerializationForBlock<TraitType::Pointer, std::shared_ptr<T>>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(std::shared_ptr<T>& object)
        {
            object.reset();
            ParserToken    tokenType = parser.getToken();
            if (tokenType == ParserToken::Value && parser.isValueNull())
            {
                parser.ignoreDataValue();
                return;
            }

            parser.pushBackToken(tokenType);

            if (parser.config.useOldSharedPtr)
            {
                tryParsePolyMorphicObject(parent, parser, object, 0);
                return;
            }

            SharedInfo<T> info;
            DeSerializationForBlock<Traits<SharedInfo<T>>::type, SharedInfo<T>> deserializer(parent, parser);
            deserializer.scanObject(info);

            parser.getShared(info, object);
        }
};
template<typename T>
class DeSerializationForBlock<TraitType::Reference, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            using Trait         = Traits<std::remove_cv_t<T>>;
            using RefType       = typename Trait::RefType;
            using ValueGetter   = typename Trait::ValueGetter;
            ValueGetter         getter(parser);
            DeSerializationForBlock<Traits<std::remove_cv_t<RefType>>::type, RefType>   deserializer(parent, parser);
            deserializer.scanObject(getter.getInputValue(object));
        }
};
/*
 * Specialization for Enum.
 * This is only used at the top level.
 * There is no open or close. Just a single value is expected.
 */
template<typename T>
class DeSerializationForBlock<TraitType::Enum, T>
{
    DeSerializer&       parent;
    ParserInterface&    parser;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
        {}
        void scanObject(T& object)
        {
            ParserToken    tokenType = parser.getToken();
            if (tokenType != ParserToken::Value)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Enum>",
                                 "DeSerializationForBlock",
                                 "Invalid Object");
            }
            std::string     objectValue;
            parser.getValue(objectValue);

            object = Traits<std::remove_cv_t<T>>::getValue(objectValue, "ThorsAnvil::Serialize::DeSerializationForBlock<Enum>::DeSerializationForBlock:");
        }
};

/*
 * Specialization for Array.
 * It is like Map expect that there are no Keys.
 * This made it different enough that combining this into a single
 * function was messy.
 */
template<typename T>
class DeSerializationForBlock<TraitType::Array, T>
{
    DeSerializer&    parent;
    ParserInterface& parser;
    std::size_t      index;
    public:
        DeSerializationForBlock(DeSerializer& parent, ParserInterface& parser)
            : parent(parent)
            , parser(parser)
            , index(-1)
        {
            ParserToken    tokenType = parser.getToken();

            if (tokenType != ParserToken::ArrayStart)
            {
                ThorsLogAndThrow("ThorsAnvil::Serialize::DeSerializationForBlock<Array>",
                                 "DeSerializationForBlock",
                                 "Invalid Object Start");
            }
        }

        void scanObject(T& object)
        {
            while (hasMoreValue())
            {
                parent.scanObjectMembers(index, object);
            }
        }
        bool hasMoreValue()
        {
            ParserToken    tokenType = parser.getToken();
            bool           result    = tokenType != ParserToken::ArrayEnd;
            if (result)
            {
                parser.pushBackToken(tokenType);
                ++index;
            }
            return result;
        }
};

/* ------------ DeSerializeMember ------------------------- */

template<typename T, typename M>
DeSerializeMemberContainer<T, M>::DeSerializeMemberContainer(DeSerializer&, ParserInterface& parser, T& object, M T::* memberInfo)
{
    DeSerializer    deSerializer(parser, false);
    deSerializer.parse(object.*(memberInfo));
}

template<typename T, typename M>
DeSerializeMemberContainer<T, M>::DeSerializeMemberContainer(DeSerializer&, ParserInterface& parser, T& /*object*/, M* memberInfo)
{
    DeSerializer    deSerializer(parser, false);
    deSerializer.parse(*memberInfo);
}

template<typename T, typename M, TraitType Type>
DeSerializeMemberValue<T, M, Type>::DeSerializeMemberValue(DeSerializer& parent, ParserInterface& parser, T& object, M T::* memberInfo)
{
    DeSerializationForBlock<Type, M>    deserializer(parent, parser);
    deserializer.scanObject(object.*(memberInfo));
}

template<typename T, typename M, TraitType Type>
DeSerializeMemberValue<T, M, Type>::DeSerializeMemberValue(DeSerializer& parent, ParserInterface& parser, T&, M* memberInfo)
{
    DeSerializationForBlock<Type, M>    deserializer(parent, parser);
    deserializer.scanObject(*memberInfo);
}

template<typename T, typename M, TraitType Type = Traits<std::remove_cv_t<M>>::type>
class DeSerializeMember: public TraitsInfo<T, M, Type>::DeSerializeMember
{
    using Parent = typename TraitsInfo<T, M, Type>::DeSerializeMember;
    public:
        using Parent::Parent;
};

template<typename T, typename M>
bool make_DeSerializeMember(DeSerializer& parent, ParserInterface& parser, std::string const& key, T& object, std::pair<char const*, M*> const& memberInfo)
{
    if (key.compare(ThorsAnvil::Serialize::Override<T>::nameOverride(memberInfo.first)) != 0) {
        return false;
    }
    DeSerializeMember<T, M>     deSerializer(parent, parser, object, memberInfo.second);
    return true;
}

template<typename T, typename M>
bool make_DeSerializeMember(DeSerializer& parent, ParserInterface& parser, std::string const& key, T& object, std::pair<char const*, M T::*> const& memberInfo)
{
    if (key.compare(ThorsAnvil::Serialize::Override<T>::nameOverride(memberInfo.first)) != 0) {
        return false;
    }
    DeSerializeMember<T, M>     deSerializer(parent, parser, object, memberInfo.second);
    return true;
}

/* ------------ DeSerializer ------------------------- */
template<typename T, typename Members, std::size_t... Seq>
inline bool DeSerializer::scanEachMember(std::string const& key, T& object, Members const& member, std::index_sequence<Seq...> const&)
{
    bool result = (make_DeSerializeMember(*this, parser, key, object, std::get<Seq>(member)) || ...);
    return result;
}

template<typename T, typename... Members>
inline bool DeSerializer::scanMembers(std::string const& key, T& object, std::tuple<Members...> const& members)
{
    return scanEachMember(key, object, members, std::make_index_sequence<sizeof...(Members)>());
}

template<typename T, typename I, typename Action>
inline bool DeSerializer::scanMembers(I const& key, T& object, Action action)
{
    action(parser, key, object);
    return true;
}

template<typename T, typename I>
inline bool DeSerializer::scanObjectMembers(I const& key, T& object)
{
    ApplyActionToParent<Traits<std::remove_cv_t<T>>::type, T, I>     parentScanner;

    bool result =  parentScanner.scanParentMember(*this, key, object)
                || scanMembers(key, object, Traits<std::remove_cv_t<T>>::getMembers());
    return result;
}

template<typename T>
inline void DeSerializer::parse(T& object)
{
    try
    {
        DeSerializationForBlock<Traits<std::remove_cv_t<T>>::type, T>     block(*this, parser);
        block.scanObject(object);
    }
    catch (std::exception const& e)
    {
        root = false;
        ThorsCatchMessage("ThorsAnvil::Serialize::DeSerializer", "parse", e.what());
        ThorsRethrowMessage("ThorsAnvil::Serialize::DeSerializer", "parse", e.what());
        throw;
    }
    catch (...)
    {
        root = false;
        ThorsCatchMessage("ThorsAnvil::Serialize::DeSerializer", "parse", "UNKNOWN");
        ThorsRethrowMessage("ThorsAnvil::Serialize::DeSerializer", "parse", "UNKNOWN");
        throw;
    }
}

/* ------------ SerializerForBlock ------------------------- */

template<TraitType traitType, typename T>
class SerializerForBlock
{
    static_assert(
        traitType != TraitType::Invalid,
        "Invalid Serialize TraitType. This usually means you have not define ThorsAnvil::Serialize::Traits<Your Type>"
    );

    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer, T const& object, bool poly = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {
            std::size_t size = 0;
            if (printer.printerUsesSize())
            {
                size = Traits<std::remove_cv_t<T>>::getPrintSize(printer, object, poly);
            }
            printer.openMap(size);
        }
        ~SerializerForBlock()
        {
            printer.closeMap();
        }
        void printMembers()
        {
            parent.printObjectMembers(object);
        }
        void printPolyMorphicMembers(std::string const& type)
        {
            printer.addKey(printer.config.polymorphicMarker);
            printer.addValue(type);
            printMembers();
        }
};

template<typename T>
class SerializerForBlock<TraitType::Value, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            printer.addValue(object);
        }
};
template<typename T>
class [[deprecated("This is caused by suing the ThorsAnvil_MakeTraitCustom macro. This is a bit hacky as it simply drops data into the stream. Please upgrade to ThorsAnvil_SelfSerialize as this allows custom behavior depending on the underlying format Json/Yaml/Bson")]]
SerializerForBlock<TraitType::Custom_Depricated, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            std::stringstream buffer;
            buffer << object;
            if (printer.printerUsesSize() && parent.isRoot())
            {
                // Force a call to getPrintSize()
                // Otherwise this is not done at the root level.
                Traits<std::remove_cv_t<T>>::getPrintSize(printer, object, false);
            }
            printer.addRawValue(buffer.str());
        }
};
template<typename T>
class SerializerForBlock<TraitType::Custom_Serialize, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            using SerializingType = typename Traits<std::remove_cv_t<T>>::SerializingType;
            SerializingType info;
            info.writeCustom(printer, object);
        }
};

/* ------------ tryPrintPolyMorphicObject Serializer ------------------------- */
template<class T>
auto tryPrintPolyMorphicObject(Serializer& parent, PrinterInterface& printer, T const& object, int) -> decltype(object->printPolyMorphicObject(parent, printer), void())
{
    // This uses a virtual method in the object to
    // call printPolyMorphicObject() the difference
    // will be the type of the template used as we will
    // get the type 'T' of the most derived type of
    // the actual runtime object.
    //
    // To install this virtual method use the macro
    // ThorsAnvil_PolyMorphicSerializer  See Traits.h for details.
    object->printPolyMorphicObject(parent, printer);
}
template<class T>
auto tryPrintPolyMorphicObject(Serializer& parent, PrinterInterface& printer, T const& object, long) -> void
{
    // This version is called if the object foes not have a virtual
    // `printPolyMorphicObject()`. Thus you get a call to the current
    // object and thus we simply use `T` and we can simply print the
    // normal members.
    using BaseType = typename BaseTypeGetter<T>::type;
    SerializerForBlock<ThorsAnvil::Serialize::Traits<std::remove_cv_t<BaseType>>::type, BaseType>  block(parent, printer, *object);
    block.printMembers();
}
/* ------------ PolyMorphic Serializer ------------------------- */
template<typename T>
void printPolyMorphicObject(Serializer& parent, PrinterInterface& printer, T const& object)
{
    using BaseType = std::remove_pointer_t<T>;
    SerializerForBlock<ThorsAnvil::Serialize::Traits<std::remove_cv_t<BaseType>>::type, BaseType>  block(parent, printer, object, true);

    // Note the call to printPolyMorphicMembers() rather than printMembers()
    // this adds the "__type": "<Type Name>"
    block.printPolyMorphicMembers(T::polyMorphicSerializerName());
}

template<typename T>
class SerializerForBlock<TraitType::Pointer, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            if (object == nullptr)
            {
                printer.addNull();
            }
            else
            {
                // Use SFINAE to call one of two versions of the function.
                tryPrintPolyMorphicObject(parent, printer, object, 0);
            }
        }
};

template<typename T>
class SerializerForBlock<TraitType::Pointer, std::shared_ptr<T>>
{
    Serializer&                 parent;
    PrinterInterface&           printer;
    std::shared_ptr<T> const&   object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer, std::shared_ptr<T> const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            if (object == nullptr)
            {
                printer.addNull();
                return;
            }

            if (printer.config.useOldSharedPtr)
            {
                tryPrintPolyMorphicObject(parent, printer, object, 0);
                return;
            }

            SharedInfo<T> info = printer.addShared(object);
            SerializerForBlock<Traits<SharedInfo<T>>::type, SharedInfo<T>>  block(parent, printer, info);
            block.printMembers();
        }
};

template<typename T>
class SerializerForBlock<TraitType::Reference, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            using Trait         = Traits<std::remove_cv_t<T>>;
            using RefType       = typename Trait::RefType;
            using ValueGetter   = typename Trait::ValueGetter;
            ValueGetter         getter(printer);
            SerializerForBlock<Traits<std::remove_cv_t<RefType>>::type, RefType>    serializer(parent, printer, getter.getOutputValue(object));
            serializer.printMembers();
        }
};

template<typename T>
class SerializerForBlock<TraitType::Enum, T>
{
    Serializer&         parent;
    PrinterInterface&   printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer,T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {}
        ~SerializerForBlock()   {}
        void printMembers()
        {
            Traits<std::remove_cv_t<T>>::serializeForBlock(printer, object);
        }
};

template<typename T>
class SerializerForBlock<TraitType::Array, T>
{
    Serializer&         parent;
    PrinterInterface& printer;
    T const&            object;
    public:
        SerializerForBlock(Serializer& parent, PrinterInterface& printer, T const& object, bool /*poly*/ = false)
            : parent(parent)
            , printer(printer)
            , object(object)
        {
            std::size_t size = 0;
            if (printer.printerUsesSize())
            {
                size = Traits<std::remove_cv_t<T>>::getPrintSize(printer, object, false);
            }
            printer.openArray(size);
        }
        ~SerializerForBlock()
        {
            printer.closeArray();
        }
        void printMembers()
        {
            parent.printObjectMembers(object);
        }
};

/* ------------ SerializeMember ------------------------- */

template<typename T, typename M>
SerializeMemberContainer<T, M>::SerializeMemberContainer(Serializer&, PrinterInterface& printer, T const& object, std::pair<char const*, M T::*> const& memberInfo)
{
    if (ThorsAnvil::Serialize::Filter<T>::filter(object, memberInfo.first, object.*(memberInfo.second)))
    {
        printer.addKey(ThorsAnvil::Serialize::Override<T>::nameOverride(memberInfo.first));

        Serializer      serialzier(printer, false);
        serialzier.print(object.*(memberInfo.second));
    }
}

template<typename T, typename M>
SerializeMemberContainer<T, M>::SerializeMemberContainer(Serializer&, PrinterInterface& printer, T const& object, std::pair<char const*, M*> const& memberInfo)
{
    if (ThorsAnvil::Serialize::Filter<T>::filter(object, memberInfo.first, *(memberInfo.second)))
    {
        printer.addKey(ThorsAnvil::Serialize::Override<T>::nameOverride(memberInfo.first));

        Serializer      serialzier(printer, false);
        serialzier.print(*(memberInfo.second));
    }
}

template<typename T, typename M, TraitType Type>
SerializeMemberValue<T, M, Type>::SerializeMemberValue(Serializer& parent, PrinterInterface& printer, T const& object, std::pair<char const*, M T::*> const& memberInfo)
{
    init(parent, printer, memberInfo.first, object, object.*(memberInfo.second));
}

template<typename T, typename M, TraitType Type>
SerializeMemberValue<T, M, Type>::SerializeMemberValue(Serializer& parent, PrinterInterface& printer, T const& object, std::pair<char const*, M*> const& memberInfo)
{
    init(parent, printer, memberInfo.first, object, *(memberInfo.second));
}

template<typename T, typename M, TraitType Type>
void SerializeMemberValue<T, M, Type>::init(Serializer& parent, PrinterInterface& printer, char const* member, T const& object, M const& value)
{
    if (ThorsAnvil::Serialize::Filter<T>::filter(object, member, value))
    {
        printer.addKey(ThorsAnvil::Serialize::Override<T>::nameOverride(member));
        SerializerForBlock<Type, M>  serializer(parent, printer, value);
        serializer.printMembers();
    }
}

template<typename T, typename M, TraitType Type = Traits<typename std::remove_cv<M>::type>::type>
class SerializeMember: public TraitsInfo<T, M, Type>::SerializeMember
{
    using Parent = typename TraitsInfo<T, M, Type>::SerializeMember;
    public:
        using Parent::Parent;
};

template<typename T, typename M>
SerializeMember<T, M> make_SerializeMember(Serializer& ser, PrinterInterface& printer, T const& object, std::pair<char const*, M*> const& memberInfo)
{
    return SerializeMember<T,M>(ser, printer, object, memberInfo);
}
template<typename T, typename M>
SerializeMember<T, M> make_SerializeMember(Serializer& ser, PrinterInterface& printer, T const& object, std::pair<char const*, M T::*> const& memberInfo)
{
    return SerializeMember<T,M>(ser, printer, object, memberInfo);
}

/* ------------ Serializer ------------------------- */

template<typename T, typename Members, std::size_t... Seq>
inline void Serializer::printEachMember(T const& object, Members const& member, std::index_sequence<Seq...> const&)
{
    (make_SerializeMember(*this, printer, object, std::get<Seq>(member)), ...);
}

template<typename T, typename... Members>
inline void Serializer::printMembers(T const& object, std::tuple<Members...> const& members)
{
    printEachMember(object, members, std::make_index_sequence<sizeof...(Members)>());
}

template<typename T, typename Action>
inline void Serializer::printMembers(T const& object, Action action)
{
    action(printer, object);
}

template<typename T>
inline void Serializer::print(T const& object)
{
    if (root && printer.preflightSize())
    {
        {
            SerializerForBlock<Traits<std::remove_cv_t<T>>::type, T>     block(*this, printer, object);
            block.printMembers();
        }
        printer.closeDoc();
        printer.reserveSize();
        printer.reset();
        printer.openDoc();
    }
    SerializerForBlock<Traits<std::remove_cv_t<T>>::type, T>     block(*this, printer, object);
    block.printMembers();
}

template<TraitType type>
struct IndexType;

template<>
struct IndexType<TraitType::Map>
{
    using IndexInfoType = std::string;
};
template<>
struct IndexType<TraitType::Array>
{
    using IndexInfoType = std::size_t;
};
template<>
struct IndexType<TraitType::Parent>
{
    using IndexInfoType = std::string;
};


template<typename T>
inline void Serializer::printObjectMembers(T const& object)
{
    printMembers(object, Traits<std::remove_cv_t<T>>::getMembers());

    using IndexInfoType = typename IndexType<Traits<std::remove_cv_t<T>>::type>::IndexInfoType;
    ApplyActionToParent<Traits<std::remove_cv_t<T>>::type, T, IndexInfoType>     parentPrinter;

    parentPrinter.printParentMembers(*this, object);
}

}

#endif
