
#ifndef THORSANVIL_SERIALIZE_JSON_SERIAlIZE_CONTAINER_H
#define THORSANVIL_SERIALIZE_JSON_SERIAlIZE_CONTAINER_H

#include "JsonSerializer.h"

namespace ThorsAnvil
{
    namespace Serialize
    {
        namespace Json
        {

/*
 * To generalize the code below we use the ContainerTraits to get type information about the container.
 */
template<typename C>
struct ContainerTraits;

/*
 * We have two main container types isConstContainer = true/false
 * isConstContainer(true):      This means the member of the container are constant. (std::map<> std::set<>) etc.
 *                              Thus we can not create a member in the container then read data directly into this member.
 *                              We must create a temporary object read the JSON data into this temporary then insert this into the container.
 * isConstContainer(false):     This means the member of the container are NOT constant (std::vector<>, std::list<>, std::deque<>)
 *                              This mean we can insert a default object into the container as soon as possable.
 *                              The JSON reading code will then de-serialize directly into the object in the container.
 */
template<typename SerializeInfo, typename C, typename CV = typename C::value_type, bool EnablePod = boost::is_fundamental<typename ContainerTraits<C>::DataType>::value, bool isConstContainer = ContainerTraits<C>::isConstContainer>
class JsonContainerImportAction;

template<typename SerializeInfo, typename C, typename CV>
class JsonContainerImportAction<SerializeInfo, C, CV, false, true>: public ThorsAnvil::Json::SaxAction
{
    // Class for reading into a const container.
    // That contains objects that are not a fundamental JSON type (not int/float/string etc)
    typedef C                                       LocalType;
    typedef typename ContainerTraits<C>::ValueType  ValueType;
    LocalType&                  destination;
    ValueType                   nextValue;

    public:
        JsonContainerImportAction(LocalType& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& parser, ThorsAnvil::Json::Key const& /*key*/)
        {
            boost::mpl::for_each<typename JsonSerializeTraits<ValueType>::SerializeInfo>(MPLForEachActivateItem<ValueType, ThorsAnvil::Json::ScannerSax>(parser, nextValue));
        }
        // Read fundamental type directly into the member
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const&)
        {
            destination.insert(nextValue);
        }
};
template<typename SerializeInfo, typename C, typename CV>
class JsonContainerImportAction<SerializeInfo, C, CV, true, true>: public ThorsAnvil::Json::SaxAction
{
    // Class for reading into a const container.
    // That contains objects that are a fundamental JSON type (int/float/string etc)
    C&            destination;
    public:
        JsonContainerImportAction(C& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& /*parser*/, ThorsAnvil::Json::Key const& /*key*/)
        {}
        // Read fundamental type directly into the member
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const& /*key*/, JsonValue const& value)
        {
            destination.insert(value.getValue<typename ContainerTraits<C>::DataType>());
        }
};
template<typename SerializeInfo, typename C, typename CV>
class JsonContainerImportAction<SerializeInfo, C, CV, false, false>: public ThorsAnvil::Json::SaxAction
{
    // Class for reading into a mutable container
    // That contains objects that are a NOT fundamental JSON type (not int/float/string etc)
    typedef C                                       LocalType;
    typedef typename ContainerTraits<C>::DataType   DataType;
    LocalType&      destination;

    public:
        JsonContainerImportAction(LocalType& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax& parser, ThorsAnvil::Json::Key const& /*key*/)
        {
            destination.push_back(DataType());
            boost::mpl::for_each<typename JsonSerializeTraits<DataType>::SerializeInfo>(MPLForEachActivateItem<DataType, ThorsAnvil::Json::ScannerSax>(parser, destination.back()));
        }
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const&)
        {}
};

template<typename SerializeInfo, typename C, typename CV>
class JsonContainerImportAction<SerializeInfo, C, CV, true, false>: public ThorsAnvil::Json::SaxAction
{
    // Class for reading into a mutable container
    // That contains objects that are a fundamental JSON type (int/float/string etc)
    typedef C                                       LocalType;
    typedef typename ContainerTraits<C>::DataType   DataType;
    LocalType&      destination;

    public:
        JsonContainerImportAction(LocalType& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&)
        {}
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const& value)
        {
            destination.push_back(value.getValue<DataType>());
        }
};

template<typename SerializeInfo, typename C>
class JsonContainerImportAction<SerializeInfo, C, std::string, false, false>: public ThorsAnvil::Json::SaxAction
{
    // Class for reading into a mutable container
    // That contains objects that are a fundamental JSON type (int/float/string etc)
    typedef C                                       LocalType;
    typedef typename ContainerTraits<C>::DataType   DataType;
    LocalType&      destination;

    public:
        JsonContainerImportAction(LocalType& dst)
            : destination(dst)
        {}

        virtual void doPreAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&)
        {}
        virtual void doAction(ThorsAnvil::Json::ScannerSax&, ThorsAnvil::Json::Key const&, JsonValue const& value)
        {
            destination.push_back(value.getValue<DataType>());
        }
};

template<typename SerializeInfo, typename C>
ThorsAnvil::Json::SaxAction* new_JsonImportAction(C& dst)
{
    return new JsonContainerImportAction<SerializeInfo,C>(dst);
}

/* Specialization of JsonContainerAttributeAccessor used for containers. */
template<typename C>
class JsonContainerAttributeAccessor
{
    public:
    void serialize(C const& src, std::ostream& stream) const
    {
        if (!src.empty())
        {
            typename C::const_iterator loop = src.begin();
            stream << jsonInternalExport(*loop);

            for(++loop; loop != src.end(); ++loop)
            {
                stream << "," << jsonInternalExport(*loop);
            }
        }
    }
    SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>      action(C& dst) const
    {
        SMART_OWNED_PTR<ThorsAnvil::Json::SaxAction>  action(new_JsonImportAction<typename JsonSerializeTraits<typename ContainerTraits<C>::DataType>::SerializeInfo>(dst));
        return action;
    }
};

        }
    }
}

#endif




