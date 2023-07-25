#ifndef THORSANVIL_CRYPTO_BASE_H
#define THORSANVIL_CRYPTO_BASE_H

#include "ThorsCryptoConfig.h"
#include "ThorsLogging/ThorsLogging.h"

namespace ThorsAnvil::Crypto
{

template<typename I>
class Base64DecodeIterator
{
    I       iter    = I{};
    int     bits    = 0;
    int     buffer  = 0;
    public:

    using difference_type   = std::ptrdiff_t;
    using value_type        = char;
    using pointer           = char*;
    using reference         = char&;
    using iterator_category = std::input_iterator_tag;

    Base64DecodeIterator()  {}
    Base64DecodeIterator(I iter)
        : iter(iter)
    {}

    // Check state of iterator.
    // We are not done until all the bits have been read even if we are at the end iterator.
    bool operator==(Base64DecodeIterator const& rhs) const  {return (iter == rhs.iter) && (bits == 0);}
    bool operator!=(Base64DecodeIterator const& rhs) const  {return !(*this == rhs);}

    // Increment Simply remove bits.
    // Note: The interface for input iterator required a * before each ++ operation.
    //       So we don't need to do any work on the ++ operator but do it all in the * operator
    Base64DecodeIterator& operator++()      {bits -= 8;return *this;}
    Base64DecodeIterator operator++(int)    {Base64DecodeIterator  result(this);++(*this);return result;}

    char operator*()
    {
        // If nothing in the buffer than fill it up.
        if (bits == 0)
        {
            static constexpr char convert[]
                    = "\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F"    //   0 - 15 00 - 0F
                      "\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F"    //  16 - 31 10 - 1F
                      "\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x8F\x3E\x8F\x8F\x8F\x3F"    //  32 - 47 20 - 2F + /
                      "\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x8F\x8F\x8F\x40\x8F\x8F"    //  48 - 63 30 - 3F 0-9
                      "\x8F\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E"    //  64 - 79 40 - 4F A-O
                      "\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x8F\x8F\x8F\x8F\x8F"    //  80 - 95 50 - 5F P-Z
                      "\x8F\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23\x24\x25\x26\x27\x28"    //  96 -111 60 - 6F a-o
                      "\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x8F\x8F\x8F\x8F\x8F";   // 112 -127 70 - 7F p-z

            int extra = 0;
            // Base64 input is based on the input being 3 input bytes => 4 output bytes.
            // There will always be a multiple of 3 bytes on the input. So read 3 bytes
            // at a time.
            while (bits != 24)
            {
                unsigned char tmp = *iter++;
                unsigned char b64 = convert[tmp & 0x7F];
                if (b64 == 0x8F || tmp > 0x7F)
                {
                    ThorsLogAndThrow("ThorsAnvil::Crypto::Base64DecodeIterator",
                                     "operator*()",
                                     "invalid input");
                }
                if (b64 == 0x40)    // We found a padding byte '='
                {
                    extra += 8;
                    b64 = 0;
                }

                buffer = (buffer << 6) | b64;
                bits  = bits + 6;
            }
            // Remove any padding bits we found.
            buffer = buffer >> extra;
            bits -= extra;
        }
        char result = (buffer >> (bits - 8)) & 0xFF;
        return result;
    }
};

template<typename I>
class Base64EncodeIterator
{
    I               iter    = I{};
    mutable int     bits    = 0;
    mutable int     buffer  = 0;
    public:

    using difference_type   = std::ptrdiff_t;
    using value_type        = char;
    using pointer           = char*;
    using reference         = char&;
    using iterator_category = std::input_iterator_tag;

    Base64EncodeIterator() {}
    Base64EncodeIterator(I iter)
        : iter(iter)
    {}
    enum Flags
    {
            EndFlag  = 0x8000,
            FillFlag = 0x4000,
            Data     = 0x3FFF,
    };

    bool operator==(Base64EncodeIterator const& rhs) const
    {
        // Note: That we have reached the end of the input stream.
        //       That means we can not read more data in the * operator.
        // Note: The input iterator interface requires you to the check
        //       the iterator against end before continuing.
        if (iter == rhs.iter)
        {
            buffer = buffer | Flags::EndFlag;
        }
        // We are not finished even if we have reached the end iterator
        // if there is still data left to decode in the buffer.
        return (iter == rhs.iter) && (bits == 0);
    }
    bool operator!=(Base64EncodeIterator const& rhs) const  {return !(*this == rhs);}

    // Increment the current position.
    Base64EncodeIterator& operator++()      {bits -= 6;return *this;}
    Base64EncodeIterator operator++(int)    {Base64EncodeIterator  result(this);++(*this);return result;}

    char operator*()
    {
        // We convert three 8 bit values int four 6 bit values.
        // But the input can be any size (i.e. it is not padded to length).
        // We must therefore detect then end of stream (see operator ==) and
        // insert the appropriate padding on the output. But this also means
        // we can not simply keep reading from the input as we cant detect
        // the end here.
        //
        // Therefor we only reads 1 byte at a time from the input. We don't
        // need to read a byte every call as we have 2 bits left over from
        // each character read thus every four call to this function will
        // return a byte without a read.
        //
        // Note this means the buffer will only ever have a maximum of 14 bits (0-13)
        // of data in it. We re-use bits 14/15 as flags. Bit 15 marks the end
        // Bit 14 indicates that we should return a padding character.

        // Check if we should return a padding character.
        bool fillFlag = buffer & Flags::FillFlag;


        if (bits < 6)
        {
            if (buffer & Flags::EndFlag)
            {
                // If we have reached the end if the input
                // we simply pad the data with 0 value in the buffer.
                // Note we add the FillFlag here so the next call
                // will be returning a padding character
                buffer = Flags::EndFlag | Flags::FillFlag | ((buffer << 8) & Flags::Data);
            }
            else
            {
                // Normal operation. Read data from the input
                // Add it to the buffer.
                unsigned char tmp = *iter++;
                buffer = ((buffer << 8) | tmp) &  Flags::Data;
            }
            bits += 8;
        }

        static constexpr char convert[]
                    = "ABCDEFGHIJKLMNOP"    // 00 - 0F
                      "QRSTUVWXYZabcdef"    // 10 - 1F
                      "ghijklmnopqrstuv"    // 20 - 2F
                      "wxyz0123456789+/";   // 30 - 3F
        // Output is either padding or converting the 6 bit value into an encoding.
        char result = fillFlag ? '=' : convert[(buffer >> (bits - 6)) & 0x3F];

        return result;
    }
};

template<typename I>
Base64DecodeIterator<I> make_decode64(I iter)
{
    return Base64DecodeIterator<I>(iter);
}
template<typename I>
Base64EncodeIterator<I> make_encode64(I iter)
{
    return Base64EncodeIterator<I>(iter);
}

}

#endif
