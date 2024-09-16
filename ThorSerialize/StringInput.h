#ifndef THORSANVIL_SERIALIZER_STRING_INPUT_H
#define THORSANVIL_SERIALIZER_STRING_INPUT_H

#include "SerializeConfig.h"
#include <string_view>
#include <string>

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

        void readValue(short int& value)                {value = readInteger();}
        void readValue(int& value)                      {value = readInteger();}
        void readValue(long int& value)                 {value = readInteger();}
        void readValue(long long int& value)            {value = readLongInteger();}

        void readValue(unsigned short int& value)       {value = readUInteger();}
        void readValue(unsigned int& value)             {value = readUInteger();}
        void readValue(unsigned long int& value)        {value = readUInteger();}
        void readValue(unsigned long long int& value)   {value = readULongInteger();}

        void readValue(float& value)                    {value = readDouble();}
        void readValue(double& value)                   {value = readDouble();}
        void readValue(long double& value)              {value = readLongDouble();}
        void readValue(char& value)
        {
            while (position < data.size() && std::isspace(data[position])) {
                ++position;
            }
            value = (position < data.size()) ? data[position] : -1;
            ++position;
        }
    private:
        long readInteger()
        {
            char* end;
            char const* start = &data[position];
            long result = std::strtol(start, &end, 10);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
        long long readLongInteger()
        {
            char* end;
            char const* start = &data[position];
            long long result = std::strtoll(start, &end, 10);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
        unsigned long readUInteger()
        {
            char* end;
            char const* start = &data[position];
            unsigned long result = std::strtoul(start, &end, 10);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
        unsigned long long readULongInteger()
        {
            char* end;
            char const* start = &data[position];
            unsigned long long result = std::strtoull(start, &end, 10);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
        double readDouble()
        {
            char* end;
            char const* start = &data[position];
            double result = std::strtod(start, &end);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
        long double readLongDouble()
        {
            char* end;
            char const* start = &data[position];
            long double result = std::strtold(start, &end);
            lastRead = (end - start);
            position += lastRead;
            return result;
        }
};

}

#endif
