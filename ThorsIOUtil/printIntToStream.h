#ifndef THORSANVIL_IOUTIL_PRINT_INT_TO_STREAM_H
#define THORSANVIL_IOUTIL_PRINT_INT_TO_STREAM_H

#include "ThorsIOUtilConfig.h"
#include "FormatInfo.h"
#include "printToStream.h"

#include <ostream>
#include <cmath>

namespace ThorsAnvil
{
    namespace IOUtil
    {

inline long long            absm(long long arg)            {return std::abs(arg);}
inline long                 absm(long arg)                 {return std::abs(arg);}
inline int                  absm(int arg)                  {return std::abs(arg);}
inline unsigned long long   absm(unsigned long long arg)   {return arg;}
inline unsigned long        absm(unsigned long arg)        {return arg;}
inline unsigned int         absm(unsigned int arg)         {return arg;}

// @function-internal
// Given an argument `arg` (which is one of the int types) and a format `info` serialize it to the stream `s`
// @param s             stream to serialize onto
// @param arg           the integer argument we want to serialize
// @param info          formatInfo object that defines how to serialize arg
template<typename T>
inline
void printIntToStream(std::ostream& s, T arg, FormatInfo const& info)
{
    static long double  const logFor16    = std::log10(16.0L);
    static long double  const logFor10    = std::log10(10.0L);
    static long double  const logFor08    = std::log10(8.0L);

    double const&  logBase = s.flags() & std::ios_base::oct ? logFor08 : s.flags() & std::ios_base::hex ? logFor16 : logFor10;

    if (info.width == 0 && info.precision == static_cast<std::size_t>(-1))
    {
        s << arg;
    }
    else
    {
        std::size_t width     = info.width;
        std::size_t precision = info.precision;

        /*
         * When precision or Width are specified the default does not do the same as C sprintf library
         * So we are going to take care of it manually here
         * So turn off the standard printing functions
         */
        s.width(0);
        s.unsetf(std::ios_base::showpos | std::ios_base::showbase);

        // extraChar    extra charters we are forced to print.  +- 0x
        // extraDigits  extra digits we are forced to print prefix 0 for octal numbers
        std::size_t extraChar       = (arg < 0) || (arg >=0 && info.forceSign && info.type == Type::Int) ? 1 : 0;
        std::size_t extraDigits     = 0;

        if (info.prefixType)
        {
            std::ios_base::fmtflags flags = s.flags() & std::ios_base::basefield;
            if (flags == std::ios_base::hex) {extraChar   += 2;}
            if (flags == std::ios_base::oct) {extraDigits += 1;}
        }
        /*
         * Number of digits to print
         * If arg is not zero use logs to calculate the number of digits.
         * If it is zero then the size is 1 unless the precision is zero (a zero value with zero precision prints nothing)
         */
        width                      = extraChar  > width ? 0 : width - extraChar;
        std::size_t numberOfDigits = (arg != 0 ? static_cast<int>((std::log10(static_cast<long double>(absm(arg))) / logBase + 1)) : (precision == 0 ? 0 : 1)) + extraDigits;
        std::size_t sizeOfNumber   = precision == static_cast<std::size_t>(-1) || numberOfDigits > precision ? numberOfDigits : precision;
        std::size_t prefix         = precision == static_cast<std::size_t>(-1) || numberOfDigits > precision ? 0 : (precision - numberOfDigits);
        std::size_t padding        = (sizeOfNumber >= width) ? 0 :  (width - sizeOfNumber);
        if (precision == static_cast<std::size_t>(-1) && info.leftPad && !info.leftJustify)
        {
            std::swap(prefix, padding);
        }

        // Add spaces before number to make it fit in width.
        if (!info.leftJustify)
        {
            for (std::size_t loop = 0; loop < padding; ++loop)
            {
                s.put(' ');
            }
        }
        // Add the - or + sign if required.
        if (arg < 0)
        {
            s.put('-');
        }
        else if (arg >=0 && info.forceSign && info.type == Type::Int)
        {
            s.put('+');
        }

        // Add the Octal or Hex prefix
        if (info.prefixType)
        {
            if (s.flags() & (std::ios_base::hex | std::ios_base::oct))
            {
                s.put('0');
            }
            if (s.flags() & std::ios_base::hex)
            {
                s.put(s.flags() & std::ios_base::uppercase ? 'X' : 'x');
            }
        }

        // Add any prefix 0 needed.
        if (prefix != static_cast<std::size_t>(-1))
        {
            for (std::size_t loop = 0; loop < prefix; ++loop)
            {
                s.put('0');
            }
        }
        else
        {
            // There is an ERROR
        }

        // Print out the absolute value (we have already printed the sign)
        // Don't print anything if precision is 0 and value is 0
        if (precision != 0 || arg != 0)
        {
            s << absm(arg);
        }
        // Add spaces after number to make it fit in width.
        if (info.leftJustify)
        {
            for (std::size_t loop = 0; loop < padding; ++loop)
            {
                s.put(' ');
            }
        }
    }
}

    }
}

#endif
