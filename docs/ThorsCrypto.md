# ThorsCrypto Developer Documentation

Header-only C++ cryptographic library in the `ThorsAnvil::Crypto` namespace. Provides hashing, HMAC, PBKDF2, SCRAM authentication, Base64 encoding/decoding, and CRC32 checksums.

All types live in `ThorsAnvil::Crypto` unless otherwise noted.

---

## Table of Contents

- [hash.h -- Hash Primitives](#hashh----hash-primitives)
  - [Type Aliases](#type-aliases)
  - [DigestStoreBase\<size\>](#digeststorebasesize)
  - [Md5 / Sha1 / Sha256](#md5--sha1--sha256)
  - [hexdigest\<Hash\>()](#hexdigesthash)
- [md5.h -- Pure C++ MD5 (RFC 1321)](#md5h----pure-c-md5-rfc-1321)
  - [Hash](#hash)
  - [MD5](#md5)
- [base64.h -- Base64 Encoding / Decoding](#base64h----base64-encoding--decoding)
  - [Base64DecodeIterator\<I\>](#base64decodeiteratori)
  - [Base64EncodeIterator\<I\>](#base64encodeiteratori)
  - [Factory Functions](#factory-functions)
- [crc.h -- CRC32 Checksums](#crch----crc32-checksums)
  - [CRC32 / CRC32C](#crc32--crc32c)
  - [CheckSum\<Type\>](#checksumtype)
- [hmac.h -- HMAC (RFC 2104)](#hmach----hmac-rfc-2104)
  - [HMacBuilder\<THash\>](#hmacbuilderthash)
  - [HMac\<THash\>](#hmacthash)
- [pbkdf2.h -- PBKDF2 (RFC 2898)](#pbkdf2h----pbkdf2-rfc-2898)
  - [Pbkdf2\<PRF\>](#pbkdf2prf)
- [scram.h -- SCRAM Authentication (RFC 5801)](#scramh----scram-authentication-rfc-5801)
  - [DBInfo\<Hi, HMAC, H, S\>](#dbinfohi-hmac-h-s)
  - [ScramMechanism\<Hi, HMAC, H, S\>](#scrammechanismhi-hmac-h-s)
  - [ScramClient\<Hi, HMAC, H, S\>](#scramclienthi-hmac-h-s)
  - [ScramServer\<Hi, HMAC, H, S\>](#scramserverhi-hmac-h-s)
  - [ScramUtil](#scramutil)
  - [Convenience Type Aliases](#convenience-type-aliases)
  - [V1 Namespace (Deprecated)](#v1-namespace-deprecated)

---

## Composition Summary

The library's generic design allows the core primitives to be composed:

```
Hash Algorithms:     Md5, Sha1, Sha256
                         |
                    HMac<THash>         -- HMAC built on any hash
                         |
                  Pbkdf2<HMac<THash>>   -- PBKDF2 built on any HMAC
                         |
        ScramClient / ScramServer       -- SCRAM built on all three
```

Each layer conforms to the same structural interface (`digestSize`, `DigestStore`, static `hash()` method), making it straightforward to plug in different algorithms.

## hash.h -- Hash Primitives

Provides lightweight wrappers around platform-native hash functions (CommonCrypto on macOS, OpenSSL elsewhere) for MD5, SHA-1, and SHA-256. Designed for hashing short data such as passwords and authentication tokens -- not for streaming large documents.

### Type Aliases

```cpp
using Byte      = char unsigned;
using DigestPtr = Byte*;

template<typename Hash>
using Digest = typename Hash::DigestStore;
```

- `Byte` -- unsigned byte type used throughout the library.
- `DigestPtr` -- raw pointer to a digest buffer. Used by `hashUnsafe` methods.
- `Digest<Hash>` -- convenience alias that resolves to the `DigestStore` type for a given hash algorithm (e.g. `Digest<Sha1>` is `DigestStoreBase<SHA_DIGEST_LENGTH>`).

### DigestStoreBase\<size\>

```cpp
template<std::size_t size>
class DigestStoreBase;
```

Fixed-size byte array that holds a hash digest result. Provides an iterator interface and implicit conversion to `DigestPtr`.

| Member | Description |
|--------|-------------|
| `operator DigestPtr()` | Implicit conversion to raw `Byte*` pointer for passing to C APIs. |
| `std::string_view view()` | Returns the digest contents as a `string_view`. |
| `Byte& operator[](std::size_t i)` | Element access (wraps around via `i % size`). |
| `begin()` / `end()` | Iterator access (both `const` and non-`const`). |

### Md5 / Sha1 / Sha256

Three stateless structs with identical interfaces, differing only in algorithm and digest size:

| Struct | `digestSize` | Underlying Algorithm |
|--------|-------------|----------------------|
| `Md5` | `MD5_DIGEST_LENGTH` (16) | MD5 |
| `Sha1` | `SHA_DIGEST_LENGTH` (20) | SHA-1 |
| `Sha256` | `SHA256_DIGEST_LENGTH` (32) | SHA-256 |

**Static Members**

| Member | Type |
|--------|------|
| `digestSize` | `static constexpr std::size_t` |
| `DigestStore` | `DigestStoreBase<digestSize>` |

**Static Methods**

| Method | Description |
|--------|-------------|
| `hash(DigestStore& src, DigestStore& dst)` | Hash a digest-sized block into `dst`. |
| `hash(std::string_view src, DigestStore& dst)` | Hash a `string_view` into `dst`. |
| `hash(std::string const& src, DigestStore& dst)` | Hash a `std::string` into `dst`. |
| `hashUnsafe(std::string_view src, DigestPtr dst)` | Hash into a raw pointer. Caller must guarantee `dst` points to at least `digestSize` bytes. |

**Example**

```cpp
#include "hash.h"
using namespace ThorsAnvil::Crypto;

Digest<Sha1> digest;
Sha1::hash(std::string("hello"), digest);
std::string hex = hexdigest<Sha1>(digest);
```

### hexdigest\<Hash\>()

```cpp
template<typename Hash>
std::string hexdigest(Digest<Hash> const& digest);

template<typename Hash>
std::string hexdigest(std::string const& message);
```

Converts a digest (or computes one from a message) into a lowercase hexadecimal string.

**Example**

```cpp
std::string hex = hexdigest<Md5>("TestString");
// hex == "5b56f40f8828701f97fa4511ddcd25fb"
```

---

## md5.h -- Pure C++ MD5 (RFC 1321)

A self-contained MD5 implementation with no external dependencies. Supports both one-shot and incremental (streaming) hashing.

### Hash

```cpp
struct Hash : std::array<std::uint32_t, 4>;
```

128-bit MD5 result stored as four 32-bit words.

| Member | Description |
|--------|-------------|
| `Hash(uint32_t, uint32_t, uint32_t, uint32_t)` | Construct from four 32-bit words. |
| `Hash(char const* str)` | Construct from a 32-character hex string (e.g. `"d41d8cd98f00b204e9800998ecf8427e"`). |
| `operator<<(ostream&, Hash const&)` | Stream insertion; outputs lowercase hex. |

### MD5

```cpp
class MD5;
```

Non-copyable. Computes an MD5 digest via incremental or one-shot interface.

| Method | Description |
|--------|-------------|
| `MD5()` | Initialize with standard MD5 constants. |
| `reset()` | Reset to initial state, allowing reuse. |
| `Hash const& digest(std::string str)` | **One-shot**: hash `str` and return the result. |
| `void add(std::string const& str)` | **Incremental**: append data. |
| `void add(uint8_t const* begin, uint8_t const* end)` | **Incremental**: append a byte range. |
| `Hash const& hash()` | **Incremental**: finalize and return the result. Throws `std::runtime_error` if `add()` is called after finalization. |

**Example -- one-shot**

```cpp
#include "md5.h"
using namespace ThorsAnvil::Crypto;

MD5 md5;
std::cout << md5.digest("abc") << std::endl;
// Output: 900150983cd24fb0d6963f7d28e17f72
```

**Example -- incremental**

```cpp
MD5 md5;
md5.add("message di");
md5.add("gest");
Hash const& result = md5.hash();
// result == Hash("f96b697d7cb7938d525a2f31aaf161d0")
```

---

## base64.h -- Base64 Encoding / Decoding

Iterator-based Base64 encoding and decoding. The iterators model `std::input_iterator` and can be used with STL algorithms and range constructors.

### Base64DecodeIterator\<I\>

```cpp
template<typename I>
class Base64DecodeIterator;
```

Wraps an input iterator `I` over Base64-encoded characters and produces decoded bytes on dereference. Reads in groups of 4 Base64 characters to produce 3 output bytes, handling `=` padding correctly.

- Throws `std::runtime_error` on invalid Base64 input.
- Equality comparison accounts for buffered bits: the iterator is only "at end" when the underlying iterator has reached end **and** all buffered bits are consumed.

### Base64EncodeIterator\<I\>

```cpp
template<typename I>
class Base64EncodeIterator;
```

Wraps an input iterator `I` over raw bytes and produces Base64-encoded characters on dereference. Reads 1 byte at a time and outputs 6-bit encoded characters, inserting `=` padding at the end of the stream.

### Factory Functions

```cpp
template<typename I>
Base64DecodeIterator<I> make_decode64(I iter);

template<typename I>
Base64EncodeIterator<I> make_encode64(I iter);
```

**Example -- decode**

```cpp
#include "base64.h"
using namespace ThorsAnvil::Crypto;

std::string encoded = "SGVsbG8gV29ybGQ=";
std::string decoded(make_decode64(encoded.begin()),
                    make_decode64(encoded.end()));
// decoded == "Hello World"
```

**Example -- encode**

```cpp
std::string raw = "any carnal pleasure";
std::string encoded(make_encode64(raw.begin()),
                    make_encode64(raw.end()));
// encoded == "YW55IGNhcm5hbCBwbGVhc3VyZQ=="
```

---

## crc.h -- CRC32 Checksums

Provides CRC32 and CRC32C (Castagnoli) checksum computation via precomputed lookup tables.

### CRC32 / CRC32C

```cpp
class CRC32;
class CRC32C;
```

Base classes that each contain a `static constexpr std::uint32_t table[256]` lookup table. `CRC32` uses the standard (ISO 3309 / ITU-T V.42) polynomial; `CRC32C` uses the Castagnoli polynomial. These are not intended to be used directly.

### CheckSum\<Type\>

```cpp
template<typename Type>
class CheckSum : public Type;
```

Incremental checksum calculator. Inherits the lookup table from `Type` (either `CRC32` or `CRC32C`).

| Method | Description |
|--------|-------------|
| `CheckSum()` | Initialize checksum state. |
| `reset()` | Reset to initial state. |
| `std::uint32_t checksum() const` | Return the current CRC32 value (XOR-finalized). |
| `void append(View const& view)` | Append any iterable byte sequence (e.g. `std::string`, `std::string_view`, `std::vector<char>`). |

**Convenience Aliases**

```cpp
using CRC32_Checksum  = CheckSum<CRC32>;
using CRC32C_Checksum = CheckSum<CRC32C>;
```

**Example**

```cpp
#include "crc.h"
using namespace ThorsAnvil::Crypto;

CRC32_Checksum crc;
crc.append(std::string("123456789"));
assert(crc.checksum() == 0xCBF43926);
```

Data can be appended incrementally across multiple `append()` calls.

---

## hmac.h -- HMAC (RFC 2104)

Keyed-Hash Message Authentication Code. Templated on a hash algorithm (e.g. `Sha1`, `Sha256`).

### HMacBuilder\<THash\>

```cpp
template<typename THash>
class HMacBuilder;
```

RAII builder that computes an HMAC. The constructor initializes the inner/outer pads from the key, and the destructor finalizes the HMAC computation into the provided digest reference.

| Method | Description |
|--------|-------------|
| `HMacBuilder(std::string_view key, DigestStore& digest, std::size_t messageSizeGuess = 1000)` | Initialize with a key and a reference to the output digest. `messageSizeGuess` pre-allocates the internal buffer. |
| `~HMacBuilder()` | Finalizes the HMAC and writes the result into `digest`. |
| `void appendData(std::string_view message)` | Append message data to the inner hash. |

Typically you do not use `HMacBuilder` directly; use `HMac` instead.

### HMac\<THash\>

```cpp
template<typename THash>
struct HMac;
```

Simple static interface for one-shot HMAC computation.

| Member | Type / Description |
|--------|--------------------|
| `digestSize` | `static constexpr std::size_t` -- same as `THash::digestSize` |
| `Hash` | `THash` |
| `DigestStore` | `typename Hash::DigestStore` |

| Static Method | Description |
|---------------|-------------|
| `hash(std::string_view key, std::string_view message, DigestStore& digest)` | Compute HMAC of `message` using `key`, storing the result in `digest`. |

**Example**

```cpp
#include "hmac.h"
using namespace ThorsAnvil::Crypto;

Digest<HMac<Sha256>> digest;
HMac<Sha256>::hash("key", "The quick brown fox jumps over the lazy dog", digest);
```

---

## pbkdf2.h -- PBKDF2 (RFC 2898)

Password-Based Key Derivation Function 2. Templated on a pseudo-random function (PRF), typically `HMac<Sha1>` or `HMac<Sha256>`.

### Pbkdf2\<PRF\>

```cpp
template<typename PRF>
struct Pbkdf2;
```

| Member | Type / Description |
|--------|--------------------|
| `digestSize` | `static constexpr std::size_t` -- same as `PRF::digestSize` |
| `DigestStore` | `typename PRF::DigestStore` |

| Static Method | Description |
|---------------|-------------|
| `hash(std::string const& password, std::string const& salt, long iter, DigestStore& digest)` | Derive a key from `password` and `salt` over `iter` iterations. |

**Example**

```cpp
#include "pbkdf2.h"
using namespace ThorsAnvil::Crypto;

using Pbkdf2HMacSha1 = Pbkdf2<HMac<Sha1>>;

Digest<Pbkdf2HMacSha1> derived;
Pbkdf2HMacSha1::hash("password", "salt", 4096, derived);
```

---

## scram.h -- SCRAM Authentication (RFC 5801)

Salted Challenge Response Authentication Mechanism for SASL/GSS-API. Implements both client and server sides of the SCRAM handshake.

All SCRAM classes are templated on four parameters:

| Parameter | Role | Typical Value |
|-----------|------|---------------|
| `Hi` | Key derivation (PBKDF2) | `Pbkdf2<HMac<Sha1>>` or `Pbkdf2<HMac<Sha256>>` |
| `HMAC` | Message authentication | `HMac<Sha1>` or `HMac<Sha256>` |
| `H` | Hash function | `Sha1` or `Sha256` |
| `S` | Salt block index | `1` (SHA-1) or `256` (SHA-256) |

### DBInfo\<Hi, HMAC, H, S\>

```cpp
template<typename Hi, typename HMAC, typename H, std::size_t S>
struct DBInfo;
```

Holds the pre-computed authentication data that a server stores per user.

| Field | Type | Description |
|-------|------|-------------|
| `saltBase64` | `std::string&` | Base64-encoded salt. |
| `saltedPassword` | `Digest<Hi>&` | PBKDF2-derived salted password. |
| `storedKey` | `Digest<H>&` | `H(clientKey)` -- stored in the database. |
| `serverKey` | `Digest<HMAC>&` | HMAC of `"Server Key"` with salted password. |
| `clientKey` | `Digest<HMAC>&` | HMAC of `"Client Key"` with salted password. |
| `iteration` | `std::size_t` | PBKDF2 iteration count. |

### ScramMechanism\<Hi, HMAC, H, S\>

```cpp
template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramMechanism;
```

Generates `DBInfo` from a plaintext password. Used during user registration or password change to produce the values that should be stored in the database.

| Method | Description |
|--------|-------------|
| `DBInfo<Hi,HMAC,H,S> makeAuthInfo(std::string const& password, std::string const& saltBase64, std::size_t iterationCount)` | Compute all derived keys from the password and salt. Returns a `DBInfo` referencing internal storage. |

**Example**

```cpp
ScramMechanism1 mechanism;
DBInfo1 info = mechanism.makeAuthInfo("pencil", "QSXCR+Q6sek8bf92", 4096);
// info.storedKey, info.serverKey, etc. are now populated
```

### ScramClient\<Hi, HMAC, H, S\>

```cpp
template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramClient;
```

Implements the client side of the SCRAM handshake.

| Method | Description |
|--------|-------------|
| `ScramClient(std::string const& user, std::string const& password, std::string nonce = ScramUtil::randomNonce())` | Construct with username, password, and optional deterministic nonce (for testing). |
| `std::string getFirstMessage()` | Produce the client-first message (e.g. `n,,n=user,r=<nonce>`). |
| `std::string getFinalMessage(std::string const& serverFirstMessage)` | Parse the server's first response, compute the proof, and return the client-final message. |
| `bool validateServer(std::string const& serverFinalMessage)` | Verify the server's final response signature. Returns `true` if the server is authentic. |

**Example -- full handshake**

```cpp
#include "scram.h"
using namespace ThorsAnvil::Crypto;

// Client side
ScramClient1 client("user", "pencil");
std::string clientFirst = client.getFirstMessage();

// ... send clientFirst to server, receive serverFirst ...

std::string clientFinal = client.getFinalMessage(serverFirst);

// ... send clientFinal to server, receive serverFinal ...

bool ok = client.validateServer(serverFinal);
```

### ScramServer\<Hi, HMAC, H, S\>

```cpp
template<typename Hi, typename HMAC, typename H, std::size_t S>
class ScramServer;
```

Implements the server side of the SCRAM handshake.

| Constructor | Description |
|-------------|-------------|
| `ScramServer(F&& dbAccess, N&& nonceGenerator)` | `dbAccess` is a callable `(std::string const& user) -> DBInfo<Hi,HMAC,H,S>` that retrieves stored auth info for a user. `nonceGenerator` produces a random server nonce. |

| Method | Description |
|--------|-------------|
| `std::string getFirstMessage(std::string const& clientFirstMessage)` | Parse the client's first message, look up the user, and return the server-first message. |
| `std::pair<bool, std::string> getFinalMessage(std::string const& clientFinalMessage)` | Verify the client's proof. Returns `{true, serverFinalMessage}` on success, `{false, ...}` on failure. |

**Example**

```cpp
// Pre-compute auth info (e.g. at registration time)
ScramMechanism1 mechanism;
DBInfo1 userInfo = mechanism.makeAuthInfo("pencil", "QSXCR+Q6sek8bf92", 4096);

// Server side of handshake
ScramServer1 server{
    [&](std::string const& /*user*/) { return userInfo; },
    []() { return ScramUtil::randomNonce(); }
};

std::string serverFirst = server.getFirstMessage(clientFirstMessage);
auto [ok, serverFinal]  = server.getFinalMessage(clientFinalMessage);
```

### ScramUtil

```cpp
struct ScramUtil;
```

Static utility functions used by the SCRAM implementation.

| Static Method | Description |
|---------------|-------------|
| `std::string getMessageBody(std::string const& section, std::string const& message)` | Extract the value for a given SCRAM message attribute (e.g. `"r="`, `"s="`, `"i="`). |
| `std::string randomNonce()` | Generate a random Base64-encoded nonce (128 bits from `std::random_device`). |
| `CalcResp calcScram(...)` | Core SCRAM proof calculation. Returns `{response, proof, serverSignature64}`. |

### Convenience Type Aliases

SHA-1 based (SCRAM-SHA-1):

```cpp
using ScramClient1      = ScramClient<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramServer1      = ScramServer<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using ScramMechanism1   = ScramMechanism<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
using DBInfo1           = DBInfo<Pbkdf2<HMac<Sha1>>, HMac<Sha1>, Sha1, 1>;
```

SHA-256 based (SCRAM-SHA-256):

```cpp
using ScramClient256    = ScramClient<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using ScramServer256    = ScramServer<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using ScramMechanism256 = ScramMechanism<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
using DBInfo256         = DBInfo<Pbkdf2<HMac<Sha256>>, HMac<Sha256>, Sha256, 256>;
```

### V1 Namespace (Deprecated)

The `ThorsAnvil::Crypto::V1` namespace contains the original SCRAM API with a different interface. These classes are **deprecated** and will eventually be removed.

| Class | V1 Name | Replacement |
|-------|---------|-------------|
| `V1::ScramClient<...>` | `V1::ScramClientSha1`, `V1::ScramClientSha256` | `ScramClient1`, `ScramClient256` |
| `V1::ScramServer<...>` | `V1::ScramServerSha1`, `V1::ScramServerSha256` | `ScramServer1`, `ScramServer256` |

Key differences in the V1 API:
- `V1::ScramServer` takes the `clientFirstMessage` in the constructor, an `iterationCount`, a `NonceGenerator`, and a `DBInfoAccess` callback that returns raw strings for password/salt.
- The new API separates concerns: `ScramMechanism` pre-computes `DBInfo`, and `ScramServer` takes a callback returning `DBInfo`.

---

