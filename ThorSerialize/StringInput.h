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
    bool                good;

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
        std::streampos  tellg()
        {
            return position;
        }
        int get()
        {
            return good ? data[position++] : EOF;
        }
        int peek()
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
            // ' ' \f \n \r \t \v
            static char isspace[] =
                "01234567" "8     EF"   // 00-15
                "01234567" "89ABCDEF"   // 16-31
                " 1234567" "89ABCDEF"   // 32-47
                "01234567" "89ABCDEF"   // 48-63
                "01234567" "89ABCDEF"   // 64-79
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF"
                "01234567" "89ABCDEF";
            while (position < data.size() && isspace[static_cast<unsigned char>(data[position])] == ' ') {
                ++position;
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
