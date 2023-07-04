#ifndef THORSANVIL_IOUTIL_PRINT_TO_STREAM_H
#define THORSANVIL_IOUTIL_PRINT_TO_STREAM_H

#include "ThorsIOUtilConfig.h"
#include "FormatInfo.h"
#include "printIntToStream.h"
#include "printStringToStream.h"

#include <ostream>
#include <type_traits>

namespace ThorsAnvil
{
    namespace IOUtil
    {

// @function-internal
// Template method for everything apart from integers
template<typename T>
inline
typename std::enable_if<!std::is_integral<T>::value>::type
printToStream(std::ostream& s, T const& arg, FormatInfo const&)
{
    // Just use the standard stream printing methods.
    // Below we special case Int and string handling (floats work as expected).
    s << arg;
}

/*
 * As a char is an integer we need to detect this case
 * and call the standard printing functions for Type::Char
 *
 * But we can also print an integer using a char in which case we need to convert the char to an integer
 * before we can print it. Below are helper functions to convert Char to Int when printing a char as an
 * an integer.
 */
// @class-internal
template<typename T>
struct CharIntConverter
{
    using Integer   = T;
};

// @class-internal
template<>
struct CharIntConverter<char>
{
    using Integer   = int;
};

// @class-internal
template<>
struct CharIntConverter<unsigned char>
{
    using Integer   = unsigned int;
};


// @function-internal
// Template method for integers
template<typename T>
inline
typename std::enable_if<std::is_integral<T>::value>::type
printToStream(std::ostream& s, T const& arg, FormatInfo const& info)
{
    if (info.type == Type::Char)
    {
        s << static_cast<char>(arg);
    }
    else
    {
        printIntToStream(s, static_cast<typename CharIntConverter<T>::Integer>(arg), info);
    }
}

// @function-internal
// Template method for strings
inline
void printToStream(std::ostream& s, char const* const& arg, FormatInfo const& info)
{
    printStringToStream(s, arg, info);
}

    }
}

#endif
