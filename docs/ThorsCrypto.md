[Home](index.html) | [Internal Documentation](internal/ThorsCrypto.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsCrypto API

Header-only C++ cryptographic library providing hashing, HMAC, PBKDF2, SCRAM authentication, Base64 encoding/decoding, and CRC32 checksums.

**Namespace:** `ThorsAnvil::Crypto`

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorsCrypto/hash.h` | MD5, SHA-1, SHA-256 hashing |
| `ThorsCrypto/md5.h` | Pure C++ MD5 implementation (no OpenSSL dependency) |
| `ThorsCrypto/base64.h` | Base64 encoding and decoding iterators |
| `ThorsCrypto/crc.h` | CRC32 and CRC32C checksums |
| `ThorsCrypto/hmac.h` | HMAC (RFC 2104) |
| `ThorsCrypto/pbkdf2.h` | PBKDF2 key derivation (RFC 2898) |
| `ThorsCrypto/scram.h` | SCRAM authentication (RFC 5801) |

---

## Hashing (hash.h)

Lightweight wrappers around platform-native hash functions (CommonCrypto on macOS, OpenSSL elsewhere).

### Available Hash Algorithms

| Struct | Digest Size | Algorithm |
|--------|-------------|-----------|
| `Md5` | 16 bytes | MD5 |
| `Sha1` | 20 bytes | SHA-1 |
| `Sha256` | 32 bytes | SHA-256 |

### Basic Usage

```cpp
#include "ThorsCrypto/hash.h"
using namespace ThorsAnvil::Crypto;

// Hash a string
Digest<Sha256> digest;
Sha256::hash(std::string("hello world"), digest);

// Get hex string
std::string hex = hexdigest<Sha256>(digest);
```

### One-Shot Hex Digest

```cpp
std::string hex = hexdigest<Md5>("TestString");
// hex == "5b56f40f8828701f97fa4511ddcd25fb"
```

---

## Pure C++ MD5 (md5.h)

Self-contained MD5 with no external dependencies. Supports one-shot and incremental hashing.

### One-Shot

```cpp
#include "ThorsCrypto/md5.h"
using namespace ThorsAnvil::Crypto;

MD5 md5;
std::cout << md5.digest("abc") << std::endl;
// Output: 900150983cd24fb0d6963f7d28e17f72
```

### Incremental

```cpp
MD5 md5;
md5.add("message di");
md5.add("gest");
Hash const& result = md5.hash();
// result == Hash("f96b697d7cb7938d525a2f31aaf161d0")
```

---

## Base64 (base64.h)

Iterator-based Base64 encoding and decoding. Works with STL algorithms and range constructors.

### Decoding

```cpp
#include "ThorsCrypto/base64.h"
using namespace ThorsAnvil::Crypto;

std::string encoded = "SGVsbG8gV29ybGQ=";
std::string decoded(make_decode64(encoded.begin()),
                    make_decode64(encoded.end()));
// decoded == "Hello World"
```

### Encoding

```cpp
std::string raw = "any carnal pleasure";
std::string encoded(make_encode64(raw.begin()),
                    make_encode64(raw.end()));
// encoded == "YW55IGNhcm5hbCBwbGVhc3VyZQ=="
```

---

## CRC32 Checksums (crc.h)

CRC32 and CRC32C (Castagnoli) checksum computation with incremental support.

```cpp
#include "ThorsCrypto/crc.h"
using namespace ThorsAnvil::Crypto;

CRC32_Checksum crc;
crc.append(std::string("123456789"));
assert(crc.checksum() == 0xCBF43926);

// Incremental
crc.reset();
crc.append(std::string("1234"));
crc.append(std::string("56789"));
assert(crc.checksum() == 0xCBF43926);
```

**Convenience Aliases:**

| Alias | Polynomial |
|-------|------------|
| `CRC32_Checksum` | Standard (ISO 3309) |
| `CRC32C_Checksum` | Castagnoli |

---

## HMAC (hmac.h)

Keyed-Hash Message Authentication Code (RFC 2104). Templated on any hash algorithm.

```cpp
#include "ThorsCrypto/hmac.h"
using namespace ThorsAnvil::Crypto;

Digest<HMac<Sha256>> digest;
HMac<Sha256>::hash("key", "The quick brown fox jumps over the lazy dog", digest);
```

---

## PBKDF2 (pbkdf2.h)

Password-Based Key Derivation Function 2 (RFC 2898). Templated on a PRF (typically `HMac<Hash>`).

```cpp
#include "ThorsCrypto/pbkdf2.h"
using namespace ThorsAnvil::Crypto;

using Pbkdf2HMacSha1 = Pbkdf2<HMac<Sha1>>;

Digest<Pbkdf2HMacSha1> derived;
Pbkdf2HMacSha1::hash("password", "salt", 4096, derived);
```

---

## SCRAM Authentication (scram.h)

Salted Challenge Response Authentication Mechanism (RFC 5801). Implements both client and server sides.

### Convenience Type Aliases

| SHA-1 Based | SHA-256 Based |
|-------------|---------------|
| `ScramClient1` | `ScramClient256` |
| `ScramServer1` | `ScramServer256` |
| `ScramMechanism1` | `ScramMechanism256` |
| `DBInfo1` | `DBInfo256` |

### Client-Side Handshake

```cpp
#include "ThorsCrypto/scram.h"
using namespace ThorsAnvil::Crypto;

ScramClient1 client("user", "pencil");

// Step 1: Send client-first message to server
std::string clientFirst = client.getFirstMessage();

// Step 2: Receive server-first message, send client-final
std::string clientFinal = client.getFinalMessage(serverFirstMessage);

// Step 3: Validate server's response
bool serverOk = client.validateServer(serverFinalMessage);
```

### Server-Side Handshake

```cpp
// Pre-compute auth info at registration time
ScramMechanism1 mechanism;
DBInfo1 userInfo = mechanism.makeAuthInfo("pencil", "QSXCR+Q6sek8bf92", 4096);

// During authentication
ScramServer1 server{
    [&](std::string const& /*user*/) { return userInfo; },
    []() { return ScramUtil::randomNonce(); }
};

std::string serverFirst = server.getFirstMessage(clientFirstMessage);
auto [ok, serverFinal] = server.getFinalMessage(clientFinalMessage);
```

---

## Composition

All primitives compose cleanly:

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
