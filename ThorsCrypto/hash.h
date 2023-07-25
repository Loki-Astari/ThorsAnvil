#ifndef THORSANVIL_CRYPTO_HASH_H
#define THORSANVIL_CRYPTO_HASH_H

#include "ThorsCryptoConfig.h"
#ifdef  __APPLE__
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#define THOR_MD5(data, len, dst)        CC_MD5(data, len, dst)
#define THOR_SHA1(data, len, dst)       CC_SHA1(data, len, dst)
#define THOR_SHA256(data, len, dst)     CC_SHA256(data, len, dst)
#else
#include <openssl/sha.h>
#include <openssl/md5.h>
#define THOR_MD5(data, len, dst)        \
do {                                    \
    MD5_CTX     ctx;                    \
    MD5_Init(&ctx);                     \
    MD5_Update(&ctx, data, len);        \
    MD5_Final(dst, &ctx);               \
}                                       \
while (false)
#define THOR_SHA1(data, len, dst)       \
do {                                    \
    SHA_CTX     ctx;                    \
    SHA1_Init(&ctx);                    \
    SHA1_Update(&ctx, data, len);       \
    SHA1_Final(dst, &ctx);              \
}                                       \
while (false)
#define THOR_SHA256(data, len, dst)     \
do {                                    \
    SHA256_CTX     ctx;                 \
    SHA256_Init(&ctx);                  \
    SHA256_Update(&ctx, data, len);     \
    SHA256_Final(dst, &ctx);            \
}                                       \
while (false)
#endif

#include <string>
#include <array>

//
// Wrapper for sha1 and sha256 hashing algorithms
//
// Provides a simple wrapper class with the appropriates types and size
// for the resulting "digest" object. Also provides several type safe
// versions of the hashing algorithm to allow multiple know types to
// be safely hashed.

namespace ThorsAnvil::Crypto
{

using Byte      = char unsigned;
using DigestPtr = Byte*;

template<std::size_t size>
class DigestStoreBase
{
    std::array<Byte, size>      data;
    public:
        using iterator = typename std::array<Byte, size>::iterator;

        operator DigestPtr()                        {return &data[0];}
        std::string_view  view()                    {return std::string_view(reinterpret_cast<char const*>(&data[0]), std::size(data));}
        Byte&             operator[](std::size_t i) {return data[i % size];}
        iterator          begin()                   {return std::begin(data);}
        iterator          end()                     {return std::end(data);}
};

template<typename Hash>
using Digest = typename Hash::DigestStore;

template<typename Hash>
std::string hexdigest(std::string const& message)
{
    Digest<Hash>    digest;
    Hash::hash(message, digest);

    std::string buffer;
    buffer.reserve(Hash::digestSize * 2);
    for (auto val: digest)
    {
        char hi = (val >> 4) & 0xF;
        hi = (hi < 10 ? '0' : 'a' - 10) + hi;
        char lo  = (val >> 0) & 0xF;
        lo = (lo < 10 ? '0' : 'a' - 10) + lo;
        buffer += hi;
        buffer += lo;
    }

    return buffer;
}

// These versions of the hashing function are good for hashing short
// amounts of text. Use these for passwords and validation hashes
// do not use them for hashing large documents.
struct Md5
{
    static constexpr std::size_t digestSize = MD5_DIGEST_LENGTH;
    using DigestStore = DigestStoreBase<MD5_DIGEST_LENGTH>;

    static void hash(DigestStore& src,             DigestStore& dst)   {THOR_MD5(src, MD5_DIGEST_LENGTH, dst);}
    static void hash(std::string_view src,         DigestStore& dst)   {THOR_MD5(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    static void hash(std::string const& src,       DigestStore& dst)   {THOR_MD5(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    // Use only if you know the destination is large enough!!
    static void hashUnsafe(std::string_view src,   DigestPtr dst)      {THOR_MD5(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
};
struct Sha1
{
    static constexpr std::size_t digestSize = SHA_DIGEST_LENGTH;
    using DigestStore = DigestStoreBase<SHA_DIGEST_LENGTH>;

    static void hash(DigestStore& src,             DigestStore& dst)   {THOR_SHA1(src, SHA_DIGEST_LENGTH, dst);}
    static void hash(std::string_view src,         DigestStore& dst)   {THOR_SHA1(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    static void hash(std::string const& src,       DigestStore& dst)   {THOR_SHA1(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    // Use only if you know the destination is large enough!!
    static void hashUnsafe(std::string_view src,   DigestPtr dst)      {THOR_SHA1(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
};
struct Sha256
{
    static constexpr std::size_t digestSize = SHA256_DIGEST_LENGTH;
    using DigestStore = DigestStoreBase<SHA256_DIGEST_LENGTH>;

    static void hash(DigestStore& src,             DigestStore& dst)   {THOR_SHA256(src, SHA256_DIGEST_LENGTH, dst);}
    static void hash(std::string_view src,         DigestStore& dst)   {THOR_SHA256(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    static void hash(std::string const& src,       DigestStore& dst)   {THOR_SHA256(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
    // Use only if you know the destination is large enough!
    static void hashUnsafe(std::string_view src,   DigestPtr dst)      {THOR_SHA256(reinterpret_cast<Byte const*>(&src[0]), std::size(src), dst);}
};

}

#endif
