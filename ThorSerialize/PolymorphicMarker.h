#ifndef THORSANVIL_SERIALIZER_POLYMORPHIC_MARKER_H
#define THORSANVIL_SERIALIZER_POLYMORPHIC_MARKER_H

#include "SerializeConfig.h"

#include <string>

namespace ThorsAnvil::Serialize::Private
{

inline
std::string const& getDefaultPolymorphicMarker()
{
    using std::string_literals::operator""s;
    static std::string const defaultPolymorphicMarker = "__type"s;
    return defaultPolymorphicMarker;
}

template <typename T>
class CheckForPoly
{
    // Define two types of character arrays, 'yes' and 'no',
    // for SFINAE test
    typedef char yes[1];
    typedef char no[2];

    // Test if class T has a member function named 'func'
    // If T has 'func', this version of test() is chosen
    template <typename C>
    static yes& test(decltype(&C::polyname));

    // Fallback test() function used if T does not have 'func'
    template <typename> static no& test(...);

    public:
        // Static constant 'value' becomes true if T has 'func',
        // false otherwise The comparison is based on the size
        // of the return type from test()
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
};

template<typename T>
std::string getPolymorphicMarker(std::string const& polymorphicMarker)
{
    if (polymorphicMarker != "") {
        return polymorphicMarker;
    }
    if constexpr (CheckForPoly<T>::value) {
        return T::polyname();
    }
    else {
        return Private::getDefaultPolymorphicMarker();
    }
}

}

#endif
