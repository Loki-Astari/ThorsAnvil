
#ifndef THORSANVIL_SERIALIZE_JSON_SERIAlIZE_MAP_H
#define THORSANVIL_SERIALIZE_JSON_SERIAlIZE_MAP_H

#include "JsonSerializer.h"
#include "JsonSerializerMemory.h"
#include "JsonSerializerContainer.h"

#include <map>

/*
 * There are a couple of specializations for std::map to make serialization to JSON more natural.
 *
 * A JSON object uses a string as a key.
 * This if we have a std::map<> that uses a std::string as a key then it maps directly to a JSON object.
 *
 * Any other type of std::map<> will be mapped as JSON array. Where each element in the array is JSON object
 * that looks like this { "first": <Key>, "second": <value> }
 */
namespace ThorsAnvil
{
    namespace Serialize
    {
        namespace Json
        {

/*
 * Default definition for map
 */
template<typename K, typename V>
struct ContainerTraits<std::map<K,V> >
{
    static bool const isConstContainer  = true;
    typedef     V               DataType;
    typedef     std::pair<K,V>  ValueType;
};
template<typename K, typename V>
struct JsonSerializeTraits<std::map<K, V> >
{
    typedef std::map<K, V>                              LocalType;
    typedef void                                        ParentType;
    static JsonSerializeType const  type    = Array;

    typedef JsonContainerAttributeAccessor<LocalType>   Accessor;
    THORSANVIL_SERIALIZE_JsonGenericArrAttributeAccess(Accessor);
    typedef boost::mpl::vector<genericAccessor>         SerializeInfo;
};

/*
 * The following are specialization for maps that use std::string as they key.
 */
template<typename V, typename A, typename RegisterKey>
struct JsonSerialize<std::map<std::string,V>, A, RegisterKey, Map>
{
    static void activate(JsonSerializeItem<std::map<std::string,V>, A, RegisterKey> const& item, std::ostream& stream, std::map<std::string,V> const& src)
    {
        item.accessor.serialize(src, stream);
    }
};

template<typename V>
class JsonContainerAttributeAccessor<std::map<std::string, V> >
{
    public:
    void serialize(std::map<std::string,V> const& src, std::ostream& stream) const
    {
        if (!src.empty())
        {
            typename std::map<std::string,V>::const_iterator loop = src.begin();
            stream << jsonInternalExport(loop->first) << ":" << jsonInternalExport(loop->second);

            for(++loop; loop != src.end(); ++loop)
            {
                stream << "," << jsonInternalExport(loop->first) << ":" << jsonInternalExport(loop->second);
            }
        }
    }
    SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>      action(std::map<std::string,V>& dst) const
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>  action(new_JsonImportAction<typename JsonSerializeTraits<V>::SerializeInfo>(dst));
        return action;
    }
};

template<typename SerializeInfo, typename V>
class JsonContainerImportAction<SerializeInfo, std::map<std::string,V>, typename std::map<std::string,V>::value_type, true, true>: public ThorsAnvil::Json::SaxAction
{
    std::map<std::string,V>&            destination;
    public:
        JsonContainerImportAction(std::map<std::string,V>& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& parser, ThorsAnvil::Json::Key const& key)
        {}
        // Read fundamental type directly into the member
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const& key, JsonValue const& value)
        {
            destination[key.mapKey] = value.getValue<V>();
        }
};

template<typename SerializeInfo, typename V>
class JsonContainerImportAction<SerializeInfo, std::map<std::string, V>, typename std::map<std::string, V>::value_type, false, true>: public ThorsAnvil::Json::SaxAction
{
    std::map<std::string,V>&            destination;
    public:
        JsonContainerImportAction(std::map<std::string,V>& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& parser, ThorsAnvil::Json::Key const& key)
        {
            boost::mpl::for_each<SerializeInfo>(MPLForEachActivateItem<V, ThorsAnvil::Json::ScannerSax>(parser, destination[key.mapKey]));
        }
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const&)
        {}
};

/* This is a specialization for maps that have string values. I'm not sure why
 * it doesn't work with the existing code, but we found that this fixed it */
template<typename SerializeInfo>
class JsonContainerImportAction<SerializeInfo, std::map<std::string,std::string>, typename std::map<std::string,std::string>::value_type, false, true>: public ThorsAnvil::Json::SaxAction
{
    std::map<std::string,std::string>&            destination;
    public:
        JsonContainerImportAction(std::map<std::string,std::string>& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&)
        {}
        // Read fundamental type directly into the member
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const& key, JsonValue const& value)
        {
            destination[key.mapKey] = value.getValue<std::string>();
        }
};

template<typename V>
struct JsonSerializeTraits<std::map<std::string, V> >
{
    typedef std::map<std::string, V>                    LocalType;
    typedef void                                        ParentType;
    static JsonSerializeType const  type    = Map;

    typedef JsonContainerAttributeAccessor<LocalType>   Accessor;
    THORSANVIL_SERIALIZE_JsonGenericMapAttributeAccess(Accessor);
    typedef boost::mpl::vector<genericAccessor>         SerializeInfo;
};


        }
    }
}

#endif




