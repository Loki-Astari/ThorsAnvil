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

}

#endif
