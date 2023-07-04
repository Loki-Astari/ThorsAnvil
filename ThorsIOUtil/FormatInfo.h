#ifndef THORSANVIL_IOUTIL_FORMAT_INFO_H
#define THORSANVIL_IOUTIL_FORMAT_INFO_H

#include "ThorsIOUtilConfig.h"
#include <iostream>
#include <typeinfo>

namespace ThorsAnvil
{
    namespace IOUtil
    {

// Enum representing the Length, specifier and type information provided by the format string
                                                                                // printf doc http://www.cplusplus.com/reference/cstdio/printf/
enum class Strictness{CPPTypeStrict, CTypeCompat};
enum class Length    {none, hh, h, l, ll, j, z, t, L};                          // Use the same values as in printf documentation so it is easy to lookup
enum class Specifier {d, i, u, o, x, X, f, F, e, E, g, G, a, A, c, s, p, n};    // Use the same values as in printf documentation so it is easy to lookup
enum class Type      {Int, UInt, Float, Char, String, Pointer, Count};          // The type we are expecting
enum class Dynamic   {None, Width, Precision, Both};                            // If width and/or precision are specified by parameter

using AllowedType = std::pair<std::type_info const*, int>;

// @class-internal
// Each conversion specifier in the format string is parsed and converted into an object of this type.
struct FormatInfo
{
    // Formatter has the text representation:
    //      %<Flag>*<Width>?[.<Precision>]?<Length>?<Specifier>
    //
    //      Flag:       - + <space> # 0
    //      Width:      {1-9}{0-9}*
    //      Precision:  {0-9}*
    //      Length:     hh h l ll j z t L
    //      Specifier:  d i u o x X f F e E g G a A c s p n

    // Flags
    bool                    leftJustify;    // -    Left-justify within the given field width; Right justification is the default (see width sub-specifier).
    bool                    forceSign;      // +    Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.
    bool                    forceSignWidth; // (space)  If no sign is going to be written, a blank space is inserted before the value.
    bool                    prefixType;     // #    Used with o, x or X specifiers the value is preceded with 0, 0x or 0X respectively for values different than zero.
                                            //      Used with a, A, e, E, f, F, g or G it forces the written output to contain a decimal point even if no more digits follow. By default, if no digits follow, no decimal point is written.
    bool                    leftPad;        // 0    Left-pads the number with zeroes (0) instead of spaces when padding is specified (see width sub-specifier).
    // Width and precision of the formatter.
    std::size_t             width;
    std::size_t             precision;
    Length                  length;
    Specifier               specifier;

    // Pre-calculate a some values based on the format string
    Type                    type;           // Calculated based on length/specifier
    Dynamic                 useDynamicSize; // Are width/precision dynamically specified.
    AllowedType             expectedType;   // Type info we expect the next argument to be based on length/specifier
    std::ios_base::fmtflags format;         // The format flags that will be applied to stream before the next argument

};

    }
}

#endif
