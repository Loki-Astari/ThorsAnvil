#ifndef THORSANVIL_SERIALIZER_STRING_INPUT_H
#define THORSANVIL_SERIALIZER_STRING_INPUT_H

#include "SerializeConfig.h"
#include <string_view>
#include <string>
#include <charconv>

#if defined(NO_STD_SUPPORT_FROM_CHAR_DOUBLE) && (NO_STD_SUPPORT_FROM_CHAR_DOUBLE >= 1)
#include "fast_float/fast_float.h"
#endif

namespace ThorsAnvil::Serialize
{

struct StringInput
{
    std::string_view const  data;
    std::size_t             position;
    std::size_t             lastRead;
    bool                    good;

    public:
        StringInput(std::string_view const& view)
            : data(view)
            , position(0)
            , lastRead(0)
            , good(true)
        {}

        bool read(char* dst, std::size_t size)
        {
            std::size_t copySize = std::min(size, data.size() - position);
            std::copy(&data[position], &data[position + copySize], dst);

            position += copySize;
            lastRead = copySize;

            return good = position <= data.size();
        }
        bool readTo(std::string& dst, char delim)
        {
            auto find = data.find(delim, position);
            if (find == std::string::npos) {
                find = data.size();
            }
            auto size = find - position;

            std::size_t start = dst.size();
            dst.resize(start + size);
            std::copy(&data[position], &data[position + size], &dst[start]);

            position += (size + 1);
            return good = position <= data.size();
        }
        std::size_t getLastReadCount() const
        {
            return lastRead;
        }
        std::streampos  tellg() const
        {
            return position;
        }
        int get()
        {
            return good ? data[position++] : EOF;
        }
        int peek() const
        {
            return data[position];
        }
        void ignore(std::size_t size)
        {
            position += size;
            good = position <= data.size();
        }
        void clear(bool newState = true)
        {
            good = newState;
        }
        void unget()
        {
            --position;
            good = position <= data.size();
        }
        bool isOk() const
        {
            return good;
        }
        void setFail()
        {
            good = false;
        }

        template<typename T>
        bool readValue(T& value)
        {
            using std::from_chars;
#if defined(NO_STD_SUPPORT_FROM_CHAR_DOUBLE) && (NO_STD_SUPPORT_FROM_CHAR_DOUBLE >= 1)
            using fast_float::from_chars;
#endif

            auto start = &data[position];
            auto result = from_chars(start, &data[0] + data.size(), value);
            if (result.ec != std::errc::invalid_argument)
            {
                lastRead = (result.ptr - start);
                position+= lastRead;
                good = position <= data.size();
                return lastRead != 0;
            }
            return false;
        }
#if defined(NO_STD_SUPPORT_FROM_CHAR_DOUBLE) && (NO_STD_SUPPORT_FROM_CHAR_DOUBLE >= 1)
        bool readValue(long double& value)
        {
            double tmp;
            bool result = readValue(tmp);
            value = tmp;
            return result;
        }
#endif

        bool readValue(char& value)
        {
            static const std::string_view space(" \t\n\r\v\f");
            position = data.find_first_not_of(space, position);
            if (position == std::string::npos) {
                position = data.size();
            }
            value = (position < data.size()) ? data[position] : -1;
            ++position;
            good = position <= data.size();
            return true;
        }
        bool rdstate()
        {
            return good;
        }
    private:
};

}

#endif
