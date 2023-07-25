#ifndef THORSANIL_CRYPTO_MD5_H
#define THORSANIL_CRYPTO_MD5_H

#include "ThorsCryptoConfig.h"
// Based on RFC 1321
// https://www.ietf.org/rfc/rfc1321.txt
// 1992

#include <string>
#include <array>
#include <vector>
#include <ostream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <bit>
#include <iostream>


namespace ThorsAnvil::Crypto
{
    struct Hash: std::array<std::uint32_t,4>
    {
        Hash(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4)
            : array{a1, a2, a3, a4}
        {}
        Hash(char const* str)
        {
            std::string     chunk;
            for (int loop = 0; loop < 4; ++loop)
            {
                chunk = std::string_view(str + loop * 8, 8);
                (*this)[loop] = std::stoul(chunk.data(), nullptr, 16);
            }
        }
        friend std::ostream& operator<<(std::ostream& str, Hash const& data)
        {
            for (auto var : data)
            {
                str << std::setw(8) << std::setfill('0') << std::hex << var;
            }
            return str;
        }
    };

    class MD5
    {
        public:
            MD5();
            MD5(MD5 const&)             = delete;
            MD5& operator=(MD5 const&)  = delete;

            void reset();

            // Original Interface.
            Hash const& digest(std::string str);

            // New Interface
            Hash const& hash();
            void add(std::string const& str);
            void add(std::uint8_t const* begin, std::uint8_t const* end);
        private:
            void addChunk(std::uint32_t const* data);
            void addBuffer();
            Hash                    result;
            bool                    complete;
            std::vector<uint8_t>    buffer;
            std::uint32_t           msgSize = 0;
    };

inline MD5::MD5()
    : result {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}
{
    buffer.reserve(64);
    reset();
}

inline void MD5::reset()
{
    result  = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    complete = false;
    msgSize = 0;
    buffer.clear();
}


inline Hash const& MD5::digest(std::string str)
{
    add(str);
    return hash();
}

inline Hash const& MD5::hash()
{
    if (complete)
    {
        return result;
    }
    complete = true;
    buffer.emplace_back(0x80);
    if (buffer.size() >= 56)
    {
        buffer.resize(64, 0x0);
        addBuffer();
        buffer.clear();
    }

    buffer.resize(64, 0x0);
    std::uint32_t*   placeForMsgSize = reinterpret_cast<std::uint32_t*>(&buffer[56]);
    (*placeForMsgSize) = msgSize;
    addBuffer();

    for (auto& var : result)
    {
        var = ((var >> 24) | ((var & 0x00FF0000) >> 8) | ((var & 0x0000FF00) << 8) | (var << 24));
    }
    return result;
}

inline void MD5::add(std::string const& str)
{
    std::uint8_t const* begin = reinterpret_cast<std::uint8_t const*>(str.data());
    std::uint8_t const* end   = begin + str.size();

    add(begin, end);
}

inline void MD5::add(std::uint8_t const* begin, std::uint8_t const* end)
{
    if (complete)
    {
        throw std::runtime_error("Can not add Data to a completed Hash");
    }
    std::uint32_t size = std::distance(begin, end);
    msgSize += (size * 8);

    if (buffer.size() != 0)
    {
        std::uint32_t needed  = 64 - buffer.size();
        std::uint32_t add     = std::min(needed, size);
        buffer.insert(std::end(buffer), begin, begin + add);
        if (buffer.size() != 64)
        {
            return;
        }
        addBuffer();
        buffer.clear();

        begin += add;
    }
    for (std::uint8_t const* endChunk = begin + 64; endChunk < end; begin = endChunk, endChunk += 64)
    {
        addChunk(reinterpret_cast<std::uint32_t const*>(&(*begin)));
    }

    buffer.insert(std::end(buffer), begin, end);
}

inline void MD5::addBuffer()
{
    addChunk(reinterpret_cast<std::uint32_t*>(buffer.data()));
}

inline void MD5::addChunk(std::uint32_t const* data)
{
    static const std::array<std::uint32_t, 64> s_array =
    {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };

    static const std::array<std::uint32_t, 64> k_array =
    {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    std::uint32_t A = result[0];
    std::uint32_t B = result[1];
    std::uint32_t C = result[2];
    std::uint32_t D = result[3];

    for (int j = 0; j < 64; j++)
    {
        std::uint32_t F, g;
        if (j < 16)
        {
            F = (B & C) | ((~B) & D);
            g = j;
        }
        else if (j < 32)
        {
            F = (B & D) | (C & (~D));
            g = (5 * j + 1) % 16;
        }
        else if (j < 48)
        {
            F = B ^ C ^ D;
            g = (3 * j + 5) % 16;
        }
        else if (j < 64)
        {
            F = C ^ (B | (~D));
            g = (7 * j) % 16;
        }
        std::uint32_t token = data[g];
        F = F + A + token + k_array[j];
        A = D;
        D = C;
        C = B;
        //B = B + std::rotl(F, s_array[j]);
        B = B + ((F << s_array[j]) | (F >> (32 - s_array[j])));
    }
    result[0] += A;
    result[1] += B;
    result[2] += C;
    result[3] += D;
}

}

#endif
