#ifndef THORSANVIL_IOUTIL_PRINT_STRING_TO_STREAM_H
#define THORSANVIL_IOUTIL_PRINT_STRING_TO_STREAM_H

#include "ThorsIOUtilConfig.h"
#include "FormatInfo.h"
#include "printToStream.h"

namespace ThorsAnvil
{
    namespace IOUtil
    {

// @function-internal
// Given an argument `arg` and a format `info` serialize it to the stream `s`
// @param s             stream to serialize onto
// @param arg           the string argument we want to serialize
// @param info          formatInfo object that defines how to serialize arg
inline
void printStringToStream(std::ostream& s, char const* const& arg, FormatInfo const& info)
{
    if (info.precision == static_cast<std::size_t>(-1))
    {
        s << arg;
    }
    else
    {
        s.width(0);
        std::size_t     padding = (info.precision >= info.width) ? 0 :  (info.width - info.precision);
        if (!info.leftJustify)
        {
            for (std::size_t loop = 0; loop < padding; ++loop)
            {
                s.put(' ');
            }
        }
        for (std::size_t loop = 0; arg[loop] != '\0' && loop < info.precision; ++loop)
        {
            s.put(arg[loop]);
        }
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
