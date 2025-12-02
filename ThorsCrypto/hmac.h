#ifndef THORSANVIL_CRYPTO_HMAC_H
#define THORSANVIL_CRYPTO_HMAC_H

#include <string>
#include "ThorsCryptoConfig.h"
#include "hash.h"

// HMAC: Keyed-Hashing for Message Authentication RFC-2104
namespace ThorsAnvil::Crypto
{

// Look in hash.h for good examples of THash
// ThorsAnvil::Crypto::Sha1
template<typename THash>
class HMacBuilder
{
    public:
        HMacBuilder(std::string_view key, THash::DigestStore& digest, std::size_t messageSizeGuess = 1000)
            : digest(digest)
        {
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
            //std::string     ipad;
            //std::string     opad;

            ipad.reserve(BLOCK_SIZE + messageSizeGuess);
            opad.reserve(BLOCK_SIZE + THash::digestSize);
            ipad.resize(BLOCK_SIZE, '\x36');
            opad.resize(BLOCK_SIZE, '\x5c');

            for (int i=0; i< BLOCK_SIZE; i++)
            {
                ipad[i] ^= SHA1_Key[i];
                opad[i] ^= SHA1_Key[i];
            }
        }

        ~HMacBuilder()
        {
            /* STEP 4 */
            opad.resize(BLOCK_SIZE + THash::digestSize);
            hasher.hashUnsafe(ipad, reinterpret_cast<Byte*>(&opad[BLOCK_SIZE]));

            /* STEP 5 */
            // Moved XOR of opad to STEP 2

            /* STEP 6 */
            // Don't need to copy the hash of ipad onto opad as we hashed
            // into the correct destination.

            /*STEP 7 */
            hasher.hash(opad, digest);
        }

        void appendData(std::string_view message)
        {
            std::copy(std::begin(message), std::end(message), std::back_inserter(ipad));
        }

    private:
        enum { BLOCK_SIZE     = 64 };

        typename THash::DigestStore&    digest;
        THash           hasher;
        std::string     ipad;
        std::string     opad;
};

template<typename THash>
struct HMac
{
    static constexpr std::size_t digestSize = THash::digestSize;
    using Hash        = THash;
    using DigestStore = typename Hash::DigestStore;

    static void hash(std::string_view key, std::string_view message, DigestStore& digest)
    {
        HMacBuilder<THash>    hmac(key, digest, std::size(message));
        hmac.appendData(message);
    }
};

}

#endif
