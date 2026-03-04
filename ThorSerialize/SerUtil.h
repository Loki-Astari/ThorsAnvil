#ifndef THORSANVIL_SERIALIZE_SER_UTIL_H
#define THORSANVIL_SERIALIZE_SER_UTIL_H

#include "SerializeConfig.h"
#include "ParserInterface.h"
#include "Traits.h"
#include "Serialize.h"

#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <deque>
#include <list>
#include <array>
#include <tuple>
#include <initializer_list>
#include <memory>
#include <cstring>
#include <functional>
#include <optional>
#include <variant>
#include <cstddef>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <sstream>

/*
 * Container Types:
 * ================
 *
 * MapLike
 * ArrayLike
 *
 * GetValueType
 * PutValueType
 * MemberInserter
 * MemberEmplacer
 * ContainerMemberExtractorInserter
 * ContainerMemberExtractorEmplacer
 *
 * Traits<std::array<T,N>>
 * Traits<std::list<T>>
 * Traits<std::vector<T, Allocator>>
 * Traits<std::deque<T, Allocator>>
 * Traits<std::pair<A,B>>
 * Traits<std::set<K>>
 * Traits<std::multiset<K>>
 * Traits<std::map<K,V>>
 *      Traits<std::map<std::string,V>>
 * Traits<std::multimap<K,V>>
 *      Traits<std::multimap<std::string,V>>
 *
 * C++ 14 Containers
 * Traits<std::tuple<Args...>>
 * Traits<std::unordered_set<K,V>>
 * Traits<std::unordered_multiset<K>>
 * Traits<std::unordered_map<K,V>>
 *      Traits<std::unordered_map<std::string,V>>
 * Traits<std::unordered_multimap<K,V>>
 *      Traits<std::unordered_multimap<std::string,V>>
 * Traits<std::initializer_list<T>>
 *
 * Traits<std::unique_ptr<T>>
 * Traits<std::reference_wrapper<T>>
 *
 * C++ 17 Containers
 * Traits<std::optional>
 * Traits<std::variant>
 */

namespace ThorsAnvil::Serialize
{

template<>
class Traits<::ThorsAnvil::Serialize::AnyBlock>
{
    public:
        static constexpr TraitType type = TraitType::AnyBlock;
        static std::size_t getPrintSize(PrinterInterface&, ::ThorsAnvil::Serialize::AnyBlock const& data, bool)   {return data.getString().size();}
};

/* Container Helper */

template<typename C, typename T>
struct MapLike
{
    static std::size_t getPrintSize(PrinterInterface& printer, C const& object, bool)
    {
        std::size_t result = printer.getSizeMap(std::size(object));
        printer.pushLevel(true);
        for (auto const& value: object)
        {
            result += std::size(value.first);
            result += Traits<std::remove_cv_t<T>>::getPrintSize(printer, value.second, false);
        }
        printer.popLevel();
        return result;
    }
};

template<typename C, typename T>
struct ArrayLike
{
    static std::size_t getPrintSize(PrinterInterface& printer, C const& object, bool)
    {
        std::size_t result = printer.getSizeArray(std::size(object));
        printer.pushLevel(false);
        for (auto const& val: object)
        {
            result += Traits<std::remove_cv_t<T>>::getPrintSize(printer, val, false);
        }
        printer.popLevel();
        return result;
    }
};


/* ------------------------------- GetValueType ------------------------------- */
/*
 * Used to retrieve a value from a stream.
 * A normal value is retrieved directly from the stream (via the parser object).
 * A compound type Map/Array is retrieved from the stream using a DeSerializer.
 */
template<typename V, TraitType type = Traits<std::remove_cv_t<V>>::type>
class GetValueType
{
    public:
        GetValueType(ParserInterface& parser, V& value)
        {
            DeSerializer            deSerializer(parser, false);
            deSerializer.parse(value);
        }
};

template<typename V>
class GetValueType<V, TraitType::Value>
{
    public:
        GetValueType(ParserInterface& parser, V& value)
        {
            if (parser.getToken() != ThorsAnvil::Serialize::ParserToken::Value)
            {
                ThorsLogAndThrowError(std::runtime_error,
                                      "ThorsAnvil::Serializer::SerMap::GetValueType",
                                      "GetValueType<Value>",
                                      "Expecting a normal value after the key");
            }
            parser.getValue(value);
        }
};

/* ------------------------------- PutValueType ------------------------------- */
/*
 * Used to put a values onto a stream.
 * A normal value is put directly onto the stream (via the printer object).
 * A compound type Map/Array is printed to the stream using a Serializer.
 */
template<typename V, TraitType type = Traits<std::remove_cv_t<V>>::type>
class PutValueType
{
    Serializer      serializer;
    public:
        PutValueType(PrinterInterface& printer)
            : serializer(printer, false)
        {}
        void putValue(V const& value)
        {
            serializer.print(value);
        }
};

template<typename V>
class PutValueType<V, TraitType::Value>
{
    PrinterInterface&   printer;
    public:
        PutValueType(PrinterInterface& printer)
            : printer(printer)
        {}

        void putValue(V const& value)
        {
            printer.addValue(value);
        }
};

/* ------------------------------- MemberInserter ------------------------------- */
/*
 * Defines how a value is inserted into a container.
 * Overloaded for each container type (no generic definition)
 */
template<typename T>
class MemberInserter;
template<typename T>
class MemberEmplacer;

/* ------------------------------- ContainerMemberExtractor ------------------------------- */
/*
 * A generic contain manipulator class
 *
 * The operator()(printer) prints all the values in the container using PutValueType class.
 * The PutValueType will call the printer directly for value types but recursively use
 * the Serializer class for Map/Array types.
 *
 * The operator()(parser) reads a single value from the parser and puts it in the container
 * using the GetValueType class. The GetValueType will call the parser directly for value types
 * but recursively use the DeSerializer class for Map/Array types.
 *
 * Note each container has differences in the insert technique thus we use a MemberInserter
 * object to do the inserting of values after they have been read. The class attempts to do this
 * using move semantics.
 */
template<typename C, typename V = typename C::value_type>
class ContainerMemberExtractorInserter
{
    public:
        constexpr ContainerMemberExtractorInserter() {}
        void operator()(PrinterInterface& printer, C const& object) const
        {
            PutValueType<V>     valuePutter(printer);
            for (auto const& loop: object)
            {
                valuePutter.putValue(loop);
            }
        }
        void operator()(ParserInterface& parser, std::size_t const& index, C& object) const
        {
            V                   data{};
            GetValueType<V>     valueGetter(parser, data);

            MemberInserter<C>   inserter(object);
            inserter.add(index, std::move(data));
        }
};
template<typename C, typename V = typename C::value_type>
class ContainerMemberExtractorEmplacer
{
    public:
        constexpr ContainerMemberExtractorEmplacer() {}
        void operator()(PrinterInterface& printer, C const& object) const
        {
            PutValueType<V>     valuePutter(printer);
            for (auto const& loop: object)
            {
                valuePutter.putValue(loop);
            }
        }
        void operator()(ParserInterface& parser, std::size_t const& index, C& object) const
        {
            MemberEmplacer<C>   extractor(object);
            V&                  data = extractor.get(index);
            GetValueType<V>     valueGetter(parser, data);
        }
};

/* ------------------------------- Traits<std::pair<F, S>> ------------------------------- */
template<typename F, typename S>
class Traits<std::pair<F, S>>
{
    public:
        static constexpr TraitType type = TraitType::Map;
        using Self = std::pair<F,S>;

        using Members = std::tuple< REP_N(THOR_TYPEACTION, 00, Self, first, second, 1) >;

        static Members const& getMembers()
        {
            static constexpr Members members{ REP_N(THOR_VALUEACTION, 00, Self, first, second, 1) };
            return members;
        }
        static std::size_t getPrintSize(PrinterInterface& printer, std::pair<F, S> const& object, bool)
        {
            std::size_t result = printer.getSizeMap(2);
            printer.pushLevel(true);
            result = result
                 + std::strlen("first")
                 + std::strlen("second")
                 + Traits<std::remove_cv_t<std::decay_t<F>>>::getPrintSize(printer, object.first, false)
                 + Traits<std::remove_cv_t<std::decay_t<S>>>::getPrintSize(printer, object.second, false);
            printer.popLevel();
            return result;
        }
};

/* ------------------------------- Traits<std::initializer_list<T>> ------------------------------- */
template<typename T>
class MemberInserter<std::initializer_list<T>>
{
    std::initializer_list<T>& container;
    public:
        MemberInserter(std::initializer_list<T>& container)
            : container(container)
        {}
        void add(std::size_t const& , T&& )
        {
            //static_assert(false, "Can not de-serialize and initializer list");
        }
};

template<typename T>
class Traits<std::initializer_list<T>>: public ArrayLike<std::initializer_list<T>, T>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::initializer_list<T>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::array<T, N>> ------------------------------- */
template<typename T, std::size_t N>
class MemberEmplacer<std::array<T, N>>
{
    std::array<T, N>& container;
    public:
        MemberEmplacer(std::array<T, N>& container)
            : container(container)
        {}
        T& get(std::size_t const& index)
        {
            return container[index];
        }
};

template<typename T, std::size_t N>
class Traits<std::array<T, N>>: public ArrayLike<std::array<T, N>, T>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorEmplacer<std::array<T, N>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::list<T>> ------------------------------- */
template<typename T, typename Allocator>
class MemberEmplacer<std::list<T, Allocator>>
{
    std::list<T, Allocator>& container;
    public:
        MemberEmplacer(std::list<T, Allocator>& container)
            : container(container)
        {}
        T& get(std::size_t const&)
        {
            container.emplace_back();
            return container.back();
        }
};

template<typename T, typename Allocator>
class Traits<std::list<T, Allocator>>: public ArrayLike<std::list<T, Allocator>, T>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorEmplacer<std::list<T, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::vector<T>> ------------------------------- */
template<typename T, typename Allocator>
class MemberEmplacer<std::vector<T, Allocator>>
{
    std::vector<T, Allocator>& container;
    public:
        MemberEmplacer(std::vector<T, Allocator>& container)
            : container(container)
        {}
        T& get(std::size_t const&)
        {
            container.emplace_back();
            return container.back();
        }
};
template<typename T, typename Allocator>
class Traits<std::vector<T, Allocator>>: public ArrayLike<std::vector<T, Allocator>, T>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorEmplacer<std::vector<T, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

template<typename Allocator>
class MemberInserter<std::vector<bool, Allocator>>
{
    std::vector<bool, Allocator>& container;
    public:
        MemberInserter(std::vector<bool, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, bool value)
        {
            container.push_back(value);
        }
};
template<typename Allocator>
class Traits<std::vector<bool, Allocator>>: public ArrayLike<std::vector<bool, Allocator>, bool>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::vector<bool, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::deque<T>> ------------------------------- */
template<typename T, typename Allocator>
class MemberEmplacer<std::deque<T, Allocator>>
{
    std::deque<T, Allocator>& container;
    public:
        MemberEmplacer(std::deque<T, Allocator>& container)
            : container(container)
        {}
        T& get(std::size_t const&)
        {
            container.emplace_back();
            return container.back();
        }
};

template<typename T, typename Allocator>
class Traits<std::deque<T, Allocator>>: public ArrayLike<std::deque<T, Allocator>, T>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorEmplacer<std::deque<T, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::set<Key>> ------------------------------- */
template<typename Key, typename Compare, typename Allocator>
class MemberInserter<std::set<Key, Compare, Allocator>>
{
    std::set<Key, Compare, Allocator>& container;
    public:
        MemberInserter(std::set<Key, Compare, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, Key&& value)
        {
            container.insert(std::forward<Key>(value));
        }
};

template<typename Key, typename Compare, typename Allocator>
class Traits<std::set<Key, Compare, Allocator>>: public ArrayLike<std::set<Key, Compare, Allocator>, Key>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::set<Key, Compare, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::unordered_set<Key>> ------------------------------- */
template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
class MemberInserter<std::unordered_set<Key, Hash, KeyEqual, Allocator>>
{
    std::unordered_set<Key, Hash, KeyEqual, Allocator>& container;
    public:
        MemberInserter(std::unordered_set<Key, Hash, KeyEqual, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, Key&& value)
        {
            container.insert(std::forward<Key>(value));
        }
};

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
class Traits<std::unordered_set<Key, Hash, KeyEqual, Allocator>>: public ArrayLike<std::unordered_set<Key, Hash, KeyEqual, Allocator>, Key>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::unordered_set<Key, Hash, KeyEqual, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::multiset<Key>> ------------------------------- */
template<typename Key, typename Compare, typename Allocator>
class MemberInserter<std::multiset<Key, Compare, Allocator>>
{
    std::multiset<Key, Compare, Allocator>& container;
    public:
        MemberInserter(std::multiset<Key, Compare, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, Key&& value)
        {
            container.insert(std::forward<Key>(value));
        }
};

template<typename Key, typename Compare, typename Allocator>
class Traits<std::multiset<Key, Compare, Allocator>>: public ArrayLike<std::multiset<Key, Compare, Allocator>, Key>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::multiset<Key, Compare, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::unordered_multiset<Key>> ------------------------------- */
template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
class MemberInserter<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>
{
    std::unordered_multiset<Key, Hash, KeyEqual, Allocator>& container;
    public:
        MemberInserter(std::unordered_multiset<Key, Hash, KeyEqual, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, Key&& value)
        {
            container.insert(std::forward<Key>(value));
        }
};

template<typename Key, typename Hash, typename KeyEqual, typename Allocator>
class Traits<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>: public ArrayLike<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>, Key>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::unordered_multiset<Key, Hash, KeyEqual, Allocator>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::map<Key, Value>> ------------------------------- */
template<typename Key,typename T, typename Compare, typename Allocator>
class MemberInserter<std::map<Key, T, Compare, Allocator>>
{
    std::map<Key, T, Compare, Allocator>& container;
    public:
        MemberInserter(std::map<Key, T, Compare, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, std::pair<Key, T>&& value)
        {
            container.insert(std::forward<std::pair<Key, T>>(value));
        }
};

template<typename Key, typename T, typename Compare, typename Allocator>
class Traits<std::map<Key, T, Compare, Allocator>>: public ArrayLike<std::map<Key, T, Compare, Allocator>, typename std::map<Key, T, Compare, Allocator>::value_type>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::map<Key, T, Compare, Allocator>, std::pair<Key, T>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/*
 * std::map<> we use a specialization when the key is a std::string.
 * This allows maps that have string keys to be represented directly by Json Map objects.
 */
template<typename Value>
class Traits<std::map<std::string, Value>>: public MapLike<std::map<std::string, Value>, Value>
{
    public:
        static constexpr TraitType type = TraitType::Map;

        class MemberExtractor
        {
            public:
                constexpr MemberExtractor(){}
                void operator()(PrinterInterface& printer, std::map<std::string, Value> const& object) const
                {
                    PutValueType<Value>     valuePutter(printer);
                    for (auto const& loop: object)
                    {
                        printer.addKey(loop.first);
                        valuePutter.putValue(loop.second);
                    }
                }
                void operator()(ParserInterface& parser, std::string const& key, std::map<std::string, Value>& object) const
                {
                    Value&                  data = object[key];
                    GetValueType<Value>     valueGetter(parser, data);
                }
        };

        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::unordered_map<Key, Value>> ------------------------------- */
template<typename Key,typename T, typename Hash, typename KeyEqual, typename Allocator>
class MemberInserter<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>
{
    std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& container;
    public:
        MemberInserter(std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, std::pair<Key, T>&& value)
        {
            container.insert(std::forward<std::pair<Key, T>>(value));
        }
};

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
class Traits<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>>: public ArrayLike<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>, typename std::unordered_map<Key, T, Hash, KeyEqual, Allocator>::value_type>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::unordered_map<Key, T, Hash, KeyEqual, Allocator>, std::pair<Key, T>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/*
 * std::unordered_map<> we use a specialization when the key is a std::string.
 * This allows unordered_maps that have string keys to be represented directly by Json Map objects.
 */
template<typename Value>
class Traits<std::unordered_map<std::string, Value>>: public MapLike<std::unordered_map<std::string, Value>, Value>
{
    public:
        static constexpr TraitType type = TraitType::Map;

        class MemberExtractor
        {
            public:
                constexpr MemberExtractor(){}
                void operator()(PrinterInterface& printer, std::unordered_map<std::string, Value> const& object) const
                {
                    PutValueType<Value>     valuePutter(printer);
                    for (auto const& loop: object)
                    {
                        printer.addKey(loop.first);
                        valuePutter.putValue(loop.second);
                    }
                }
                void operator()(ParserInterface& parser, std::string const& key, std::unordered_map<std::string, Value>& object) const
                {
                    Value                   data{};
                    GetValueType<Value>     valueGetter(parser, data);
                    object.insert(std::make_pair(std::move(key), std::move(data)));
                }
        };

        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::unordered_multimap<Key, Value>> ------------------------------- */
template<typename Key,typename T, typename Hash, typename KeyEqual, typename Allocator>
class MemberInserter<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>>
{
    std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>& container;
    public:
        MemberInserter(std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, std::pair<Key, T>&& value)
        {
            container.insert(std::forward<std::pair<Key, T>>(value));
        }
};

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
class Traits<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>>: public ArrayLike<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>, typename std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>::value_type>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>, std::pair<Key, T>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

template<typename Value>
class Traits<std::unordered_multimap<std::string, Value>>: public MapLike<std::unordered_multimap<std::string, Value>, Value>
{
    public:
        static constexpr TraitType type = TraitType::Map;

        class MemberExtractor
        {
            public:
                constexpr MemberExtractor(){}
                void operator()(PrinterInterface& printer, std::unordered_multimap<std::string, Value> const& object) const
                {
                    PutValueType<Value>     valuePutter(printer);
                    for (auto const& loop: object)
                    {
                        printer.addKey(loop.first);
                        valuePutter.putValue(loop.second);
                    }
                }
                void operator()(ParserInterface& parser, std::string const& key, std::unordered_multimap<std::string, Value>& object) const
                {
                    Value                   data{};
                    GetValueType<Value>     valueGetter(parser, data);
                    object.insert(std::make_pair(std::move(key), std::move(data)));
                }
        };

        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::multimap<Key, Value>> ------------------------------- */
template<typename Key,typename T, typename Compare, typename Allocator>
class MemberInserter<std::multimap<Key, T, Compare, Allocator>>
{
    std::multimap<Key, T, Compare, Allocator>& container;
    public:
        MemberInserter(std::multimap<Key, T, Compare, Allocator>& container)
            : container(container)
        {}
        void add(std::size_t const&, std::pair<Key, T>&& value)
        {
            container.insert(std::forward<std::pair<Key, T>>(value));
        }
};

template<typename Key, typename T, typename Compare, typename Allocator>
class Traits<std::multimap<Key, T, Compare, Allocator>>: public ArrayLike<std::multimap<Key, T, Compare, Allocator>, typename std::multimap<Key, T, Compare, Allocator>::value_type>
{
    public:
        static constexpr TraitType type = TraitType::Array;
        using MemberExtractor = ContainerMemberExtractorInserter<std::multimap<Key, T, Compare, Allocator>, std::pair<Key, T>>;
        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

template<typename Value>
class Traits<std::multimap<std::string, Value>>: public MapLike<std::multimap<std::string, Value>, Value>
{
    public:
        static constexpr TraitType type = TraitType::Map;

        class MemberExtractor
        {
            public:
                constexpr MemberExtractor(){}
                void operator()(PrinterInterface& printer, std::multimap<std::string, Value> const& object) const
                {
                    PutValueType<Value>     valuePutter(printer);
                    for (auto const& loop: object)
                    {
                        printer.addKey(loop.first);
                        valuePutter.putValue(loop.second);
                    }
                }
                void operator()(ParserInterface& parser, std::string const& key, std::multimap<std::string, Value>& object) const
                {
                    Value                   data{};
                    GetValueType<Value>     valueGetter(parser, data);
                    object.insert(std::make_pair(std::move(key), std::move(data)));
                }
        };

        static MemberExtractor const& getMembers()
        {
            static constexpr MemberExtractor    memberExtractor;
            return memberExtractor;
        }
};

/* ------------------------------- Traits<std::tupple<Type...>> ------------------------------- */

/*
 * This is the object that parses/prints data from/to the stream
 */
template<typename... Args>
class ContainerTuppleExtractor
{
        using C = std::tuple<Args...>;

        template<std::size_t index, typename V>
        void printTupleValue(PrinterInterface& printer, C const& object) const
        {
            PutValueType<V>     valuePutter(printer);
            valuePutter.putValue(std::get<index>(object));
        }
        template<std::size_t... index>
        void printTupleValues(PrinterInterface& printer, C const& object, std::index_sequence<index...> const&) const
        {
            (printTupleValue<index, std::tuple_element_t<index, C>>(printer, object),...);
        }
        template<std::size_t index, typename V>
        void parseTupleValue(ParserInterface& parser, C& object) const
        {
            V&                  data(std::get<index>(object));
            GetValueType<V>     valueGetter(parser, data);
        }
        template<std::size_t... index>
        void parseTupleValues(ParserInterface& parser, std::size_t const& id, C& object,  std::index_sequence<index...> const&) const
        {
            using MemberDecoder = decltype(&ContainerTuppleExtractor::parseTupleValue<0, std::tuple_element_t<0, C>>);
            static std::initializer_list<MemberDecoder> parseTuppleValue = {&ContainerTuppleExtractor::parseTupleValue<index, std::tuple_element_t<index, C>>...};
            auto iteratorToFunction = parseTuppleValue.begin() + id;
            auto function = *iteratorToFunction;
            (this->*function)(parser, object);
        }
    public:
        constexpr ContainerTuppleExtractor() {}
        void operator()(PrinterInterface& printer, C const& object) const
        {
            printTupleValues(printer, object, std::make_index_sequence<sizeof...(Args)>());
        }
        void operator()(ParserInterface& parser, std::size_t const& index, C& object) const
        {
            parseTupleValues(parser, index, object, std::make_index_sequence<sizeof...(Args)>());
        }
};

/*
 * A traits class for tupple that uses the above ContainerTuppleExtractor
 */
template<typename... Args>
class Traits<std::tuple<Args...>>
{
    public:
        static constexpr TraitType type = TraitType::Array;

        static ContainerTuppleExtractor<Args...> const& getMembers()
        {
            static constexpr ContainerTuppleExtractor<Args...> members;
            return members;
        }

        template<typename E>
        static std::size_t getPrintSizeElement(PrinterInterface& printer, E const& object)
        {
            return Traits<std::remove_cv_t<E>>::getPrintSize(printer, object, false);
        }

        template<std::size_t... Seq>
        static std::size_t getPrintSizeAllElement(PrinterInterface& printer, std::tuple<Args...> const& object, std::index_sequence<Seq...> const&)
        {
            std::size_t result = (getPrintSizeElement(printer, std::get<Seq>(object)) + ...);
            return result;
        }
        static std::size_t getPrintSize(PrinterInterface& printer, std::tuple<Args...> const& object, bool)
        {
            std::size_t result = printer.getSizeArray(sizeof...(Args));
            printer.pushLevel(false);
            result += getPrintSizeAllElement(printer, object, std::make_index_sequence<sizeof...(Args)>());
            printer.popLevel();
            return result;
        }
};


template<typename T>
struct BaseTypeGetter<std::unique_ptr<T>>
{
    using ExactType = typename std::unique_ptr<T>::element_type;
    using type = std::remove_cv_t<ExactType>;
};
template<typename T>
class Traits<std::unique_ptr<T>>
{
    public:
        static constexpr TraitType type = TraitType::Pointer;
        static std::unique_ptr<T>   alloc()         {return std::make_unique<T>();}
        static void release(std::unique_ptr<T>& p)  {p.reset();}
        static std::size_t getPrintSize(PrinterInterface& printer, std::unique_ptr<T> const& object, bool)
        {
            if (object)
            {
                return Traits<std::remove_cv_t<T>>::getPrintSize(printer, *object, true);
            }
            return printer.getSizeNull();
        }
};

template<typename T>
struct BaseTypeGetter<std::shared_ptr<T>>
{
    using ExactType = typename std::shared_ptr<T>::element_type;
    using type = std::remove_cv_t<ExactType>;
};
template<typename T>
class Traits<std::shared_ptr<T>>
{
    public:
        static constexpr TraitType type = TraitType::Pointer;
        static std::shared_ptr<T>   alloc()         {return std::make_shared<T>();}
        static void release(std::shared_ptr<T>& p)  {p.reset();}
        static std::size_t getPrintSize(PrinterInterface& printer, std::shared_ptr<T> const& object, bool)
        {
            if (object == nullptr) {
                return printer.getSizeNull();
            }

            SharedInfo<T>      info = printer.addShared(object);
            return Traits<SharedInfo<T>>::getPrintSize(printer, info, true);
        }
};

template<typename T>
class Traits<std::reference_wrapper<T>>
{
    public:
        using RefType = T;
        struct ValueGetter
        {
            ValueGetter(PrinterInterface&)   {}
            ValueGetter(ParserInterface&)    {}
            T const&    getOutputValue(std::reference_wrapper<T> const& output) const {return output.get();}
            T&          getInputValue(std::reference_wrapper<T>& input)         const {return input.get();}
        };
        static constexpr TraitType type = TraitType::Reference;
        static std::size_t getPrintSize(PrinterInterface& printer, std::reference_wrapper<T> const& object, bool p)
        {
            return Traits<std::remove_cv_t<T>>::getPrintSize(printer, object, p);
        }
};

template<typename T>
class Traits<std::optional<T>>
{
    public:
        using RefType = T;
        struct ValueGetter
        {
            ValueGetter(PrinterInterface&)  {}
            ValueGetter(ParserInterface&)   {}
            T const&    getOutputValue(std::optional<T> const& output) const    {return output.value();}
            T&          getInputValue(std::optional<T>& input)         const    {if (!input.has_value()){input.emplace();}return input.value();}
        };
        static constexpr TraitType type = TraitType::Reference;
        static std::size_t getPrintSize(PrinterInterface& printer, std::optional<T> const& object, bool p)
        {
            return (object.has_value())
                ? Traits<std::remove_cv_t<T>>::getPrintSize(printer, object.value(), p)
                : 0;
        }
};

struct SizeVisitor
{
    PrinterInterface& printer;
    bool pval;
    SizeVisitor(PrinterInterface& printer, bool pval)
        : printer(printer)
        , pval(pval)
    {}
    template<typename T>
    std::size_t operator()(T const& value) const
    {
        return Traits<std::remove_cv_t<T>>::getPrintSize(printer, value, pval);
    }
};
template<typename... Args>
class Traits<std::variant<Args...>>
{
    public:
        static constexpr TraitType type = TraitType::Variant;
        static std::size_t getPrintSize(PrinterInterface& printer, std::variant<Args...> const& object, bool p)
        {
            return std::visit(SizeVisitor{printer, p}, object);
        }
};

}


#endif
