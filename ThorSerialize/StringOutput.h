#ifndef THORSANVIL_SERIALIZER_STRING_OUTPUT_H
#define THORSANVIL_SERIALIZER_STRING_OUTPUT_H

#include "SerializeConfig.h"
#include <string>
#include <charconv>

namespace ThorsAnvil::Serialize
{

struct StringOutput
{
    std::string&    data;
    std::size_t     calcSize;
    bool            ok;
    bool            preFlight;

    public:
        StringOutput(std::string& output)
            : data(output)
            , calcSize(0)
            , ok(true)
            , preFlight(false)
        {}
        bool write(char const* src, std::size_t size)
        {
            if (preFlight)
            {
                calcSize += size;
                return true;
            }
            data += std::string_view(src, size);
            return true;
        }
        bool isOk() const
        {
            return ok;
        }
        void setFail()
        {
            ok = false;
        }
        template<typename T>
        void writeValue(T const& src)
        {
            using std::to_chars;
            static char buffer[100];
            std::to_chars_result    result = to_chars(buffer, buffer + 100, src);
            if (preFlight)
            {
                calcSize += (result.ptr - buffer);
            }
            else
            {
                data += std::string_view(buffer, (result.ptr - buffer));
            }
        }
        bool preflightSize()
        {
            preFlight = true;
            return true;
        }
        void reserveSize()
        {
            data.reserve(calcSize + 10);
            preFlight = false;
        }
};

}

#endif
