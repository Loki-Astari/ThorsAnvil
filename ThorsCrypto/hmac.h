#ifndef THORSANVIL_CRYPTO_HMAC_H
#define THORSANVIL_CRYPTO_HMAC_H

#include "ThorsCryptoConfig.h"
#include "hash.h"

// HMAC: Keyed-Hashing for Message Authentication RFC-2104
namespace ThorsAnvil::Crypto
{

// Look in hash.h for good examples of THash
// ThorsAnvil::Crypto::Sha1
template<typename THash>
struct HMac
{
    static constexpr std::size_t digestSize = THash::digestSize;
    using Hash        = THash;
    using DigestStore = typename Hash::DigestStore;

    static void hash(std::string_view key, std::string_view message, DigestStore& digest)
    {
        Hash    hasher;

        enum { BLOCK_SIZE     = 64 };

        /* STEP 1 */
        std::array<Byte, BLOCK_SIZE>   SHA1_Key{'\x00'};
        if (key.size() > BLOCK_SIZE)
        {
            hasher.hashUnsafe(key, &SHA1_Key[0]);
        }
        else
        {
            std::copy(std::begin(key), std::end(key), &SHA1_Key[0]);
        }

        /* STEP 2 */
        std::string     ipad;
        std::string     opad;

        ipad.reserve(BLOCK_SIZE + std::size(message));
        opad.reserve(BLOCK_SIZE + digestSize);
        ipad.resize(BLOCK_SIZE, '\x36');
        opad.resize(BLOCK_SIZE, '\x5c');

        for (int i=0; i< BLOCK_SIZE; i++)
        {
            ipad[i] ^= SHA1_Key[i];
            opad[i] ^= SHA1_Key[i];
        }

        /* STEP 3 */
        std::copy(std::begin(message), std::end(message), std::back_inserter(ipad));

        /* STEP 4 */
        opad.resize(BLOCK_SIZE + digestSize);
        hasher.hashUnsafe(ipad, reinterpret_cast<Byte*>(&opad[BLOCK_SIZE]));

        /* STEP 5 */
        // Moved XOR of opad to STEP 2

        /* STEP 6 */
        // Don't need to copy the hash of ipad onto opad as we hashed
        // into the correct destination.

        /*STEP 7 */
        hasher.hash(opad, digest);
    }
};

}

#endif
