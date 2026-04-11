#ifndef THORSANVIL_CRYPTO_CRYPT_STRING_H
#define THORSANVIL_CRYPTO_CRYPT_STRING_H

#include <openssl/crypto.h>
#include <string>
#include <memory>

namespace ThorsAnvil::Crypto
{

template<typename T>
struct SecureAllocator
{
    using value_type = T;

    SecureAllocator() = default;
    template<typename U>
    SecureAllocator(const SecureAllocator<U>&) noexcept {}

    T* allocate(std::size_t n)
    {
        return static_cast<T*>(::operator new (n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
        OPENSSL_cleanse(p, n * sizeof(T));
        ::operator delete (p);
    }

    template<typename U>
    bool operator==(const SecureAllocator<U>&) const noexcept { return true; }
};

using String = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;

}

#endif
