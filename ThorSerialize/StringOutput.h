#ifndef THORSANVIL_SERIALIZER_STRING_OUTPUT_H
#define THORSANVIL_SERIALIZER_STRING_OUTPUT_H

#include "SerializeConfig.h"
#include "PrinterConfig.h"
#include <string>
#include <charconv>
#include <iostream>

namespace ThorsAnvil::Serialize
{

struct StringOutput
{
    std::string&    data;
    char*           current;
    char*           end;
    std::size_t     calcSize;
    bool            ok;
    bool            preFlight;
    bool            estimateSize;

    public:
        StringOutput(std::string& output, bool estimateSize)
            : data(output)
            , current(nullptr)
            , end(nullptr)
            , calcSize(0)
            , ok(true)
            , preFlight(false)
            , estimateSize(estimateSize)
        {}
        bool write(char const* src, std::size_t size)
        {
            if (preFlight)
            {
                calcSize += size;
                return true;
            }
            current = std::copy(src, src + size, current);
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
            if (preFlight)
            {
                if (estimateSize) {
                    calcSize += 25;
                }
                else
                {
                    static char buffer[100];
                    auto result = to_chars(buffer, buffer + 100, src);
                    calcSize += (result.ptr - buffer);
                }
            }
            else
            {
                auto result = to_chars(current, end, src);
                current = result.ptr;
            }
        }
        bool preflightSize()
        {
            preFlight = true;
            return true;
        }
        void reserveSize()
        {
            data.resize(calcSize);
            current = &data[0];
            end = current + calcSize;
            preFlight = false;
        }
        void finalizePrint()
        {
            // Actual size the string
            // If we are using estimateSize this may have been large.
            if (!preFlight) {
                data.resize(current - &data[0]);
            }
        }
};

}

#endif
