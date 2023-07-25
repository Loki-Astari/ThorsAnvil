#ifndef THORSANVIL_CRYPTO_PBKDF2_H
#define THORSANVIL_CRYPTO_PBKDF2_H

#include "ThorsCryptoConfig.h"
#include "hmac.h"
#include <string>

// RFC-2898 PKCS #5: Password-Based Cryptography Specification Version 2.0

namespace ThorsAnvil::Crypto
{

// Look in hmac.h for good examples of PRF
// ThorsAnvil::Crypto::HMac
template<typename PRF>
struct Pbkdf2
{
    static constexpr std::size_t digestSize = PRF::digestSize;
    using DigestStore    = typename PRF::DigestStore;

    static void hash(std::string const& password, std::string const& salt, long iter, DigestStore& digest)
    {
        using namespace std::string_literals;
        PRF             prf;
        DigestStore     tmp;

        prf.hash(password, salt + "\x00\x00\x00\x01"s, tmp);
        std::copy(std::begin(tmp), std::end(tmp), std::begin(digest));

        for (int loop = 1; loop < iter; ++loop)
        {
            prf.hash(password, tmp.view(), tmp);
            for (std::size_t loop = 0; loop < digestSize; ++loop)
            {
                digest[loop] = digest[loop] ^ tmp[loop];
            }
        }
    }
};

}

#endif
