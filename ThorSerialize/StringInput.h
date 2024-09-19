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
    std::string_view    data;
    std::size_t         position;
    std::size_t         lastRead;

    public:
        StringInput(std::string_view const& view)
            : data(view)
            , position(0)
            , lastRead(0)
        {}

        bool read(char* dst, std::size_t size)
        {
            std::size_t copySize = std::min(size, data.size() - position);
            std::copy(&data[position], &data[position + copySize], dst);

            position += copySize;
            lastRead = copySize;

            return position <= data.size();
        }
        bool readTo(std::string& dst, char delim)
        {
            auto find = data.find(delim, position);
            if (find == std::string::npos) {
                find = data.size();
            }
            auto size = find - position;

            dst.resize(size);
            std::copy(&data[position], &data[position + size], &dst[0]);

            position += (size + 1);
            return position <= data.size();
        }
        std::size_t getLastReadCount() const
        {
            return lastRead;
        }
        std::streampos  getPos()
        {
            return position;
        }
        int get()
        {
            return data[position++];
        }
        int peek()
        {
            return data[position];
        }
        void ignore(std::size_t size)
        {
            position += size;
        }
        void clear()
        {
            position = 0;
        }
        void unget()
        {
            --position;
        }
        bool isOk() const
        {
            return position <= data.size();
        }
        void setFail()
        {
            position = data.size() + 1;
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
                return true;
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
            while (position < data.size() && std::isspace(data[position])) {
                ++position;
            }
            value = (position < data.size()) ? data[position] : -1;
            ++position;
            return true;
        }
    private:
};

}

#endif
