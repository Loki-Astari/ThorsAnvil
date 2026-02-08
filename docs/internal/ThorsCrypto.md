[Home](../index.html) | [API Documentation](../ThorsCrypto.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsCrypto Internal Documentation

Detailed implementation notes for the ThorsCrypto library.

**Source:** `third/ThorsCrypto/src/ThorsCrypto/`

---

## File Map

| File | Purpose |
|------|---------|
| `hash.h` | Platform-native hash wrappers (CommonCrypto / OpenSSL) |
| `md5.h` | Pure C++ MD5 implementation (RFC 1321) |
| `base64.h` | Iterator-based Base64 encoding/decoding |
| `crc.h` | CRC32/CRC32C with precomputed lookup tables |
| `hmac.h` | HMAC (RFC 2104) |
| `pbkdf2.h` | PBKDF2 (RFC 2898) |
| `scram.h` | SCRAM authentication (RFC 5801) |

---

## Composition Architecture

The library's generic design allows core primitives to be composed:

```
Hash Algorithms:     Md5, Sha1, Sha256
                         |
                    HMac<THash>         -- HMAC built on any hash
                         |
                  Pbkdf2<HMac<THash>>   -- PBKDF2 built on any HMAC
                         |
        ScramClient / ScramServer       -- SCRAM built on all three
```

Each layer conforms to the same structural interface (`digestSize`, `DigestStore`, static `hash()` method).

---

## hash.h Implementation

### DigestStoreBase\<size\>

Fixed-size byte array holding a hash digest result:

```cpp
template<std::size_t size>
class DigestStoreBase;
```

| Member | Description |
|--------|-------------|
| `operator DigestPtr()` | Implicit conversion to raw `Byte*` for C APIs |
| `std::string_view view()` | Returns digest contents as `string_view` |
| `Byte& operator[](std::size_t i)` | Element access (wraps around via `i % size`) |
| `begin()` / `end()` | Iterator access |

### Platform Abstraction

On macOS, hash functions use CommonCrypto (`CC_MD5`, `CC_SHA1`, `CC_SHA256`). On other platforms, they use OpenSSL (`MD5()`, `SHA1()`, `SHA256()`).

### hexdigest Implementation

Converts each byte to two lowercase hex characters using a `std::stringstream` with `std::hex` and `std::setw(2)`.

---

## md5.h Implementation (RFC 1321)

### Hash Type

```cpp
struct Hash : std::array<std::uint32_t, 4>;
```

128-bit MD5 result stored as four 32-bit words. The hex string constructor parses 8 characters per word.

### MD5 State Machine

The MD5 class maintains:
- `m_state`: four 32-bit accumulator words (A, B, C, D), initialized to the MD5 constants
- `m_buffer`: internal buffer for partial blocks (64 bytes)
- `m_count`: total bits processed

**Processing steps:**
1. `add()` appends data to the internal buffer, processing complete 64-byte blocks via `processBlock()`.
2. `hash()` pads the message (1-bit, zeros, 64-bit length), processes final blocks, and returns the result.
3. `processBlock()` implements the four rounds of MD5 transformation (F, G, H, I functions) with the standard sine-derived constants.

The implementation is non-copyable. Calling `add()` after `hash()` throws `std::runtime_error`.

---

## base64.h Implementation

### Base64DecodeIterator\<I\>

Maintains internal state:
- `nextByte`: current decoded byte
- `bitsLeft`: number of valid bits remaining in the buffer
- `atEnd`: whether the underlying iterator has reached end

Reads 4 Base64 characters to produce 3 decoded bytes. Handles `=` padding. Throws `std::runtime_error` on invalid input.

Equality comparison: the iterator is "at end" only when the underlying iterator has reached end **and** all buffered bits are consumed.

### Base64EncodeIterator\<I\>

Maintains internal state:
- `nextByte`: current encoded character
- `bitsLeft`: number of valid bits remaining
- `atEnd`: end of input detected

Reads 1 byte at a time and outputs 6-bit encoded characters. Inserts `=` padding at the end of the stream.

---

## crc.h Implementation

### Lookup Table Generation

`CRC32` and `CRC32C` each contain a `static constexpr std::uint32_t table[256]` computed at compile time using the respective polynomials.

### CheckSum\<Type\> State

Maintains a running CRC value initialized to `0xFFFFFFFF`. Each `append()` call XORs each byte with the low byte of the accumulator and looks up the next value in the table. `checksum()` returns the XOR-finalized result.

---

## hmac.h Implementation

### HMacBuilder\<THash\>

RAII builder. The constructor:
1. If the key is longer than the block size, hashes it first.
2. Constructs inner pad (key XOR 0x36) and outer pad (key XOR 0x5C).
3. Initializes the internal buffer with the inner pad.

`appendData()` appends message data.

The destructor:
1. Hashes (inner pad + message) to get the inner hash.
2. Hashes (outer pad + inner hash) to get the final HMAC.
3. Writes the result into the provided `digest` reference.

---

## pbkdf2.h Implementation

Implements the PBKDF2 algorithm:
1. `U_1 = HMAC(password, salt || INT_32_BE(1))`
2. `U_i = HMAC(password, U_{i-1})`
3. `DK = U_1 XOR U_2 XOR ... XOR U_iter`

The salt is concatenated with a 4-byte big-endian block index (the `S` template parameter).

---

## scram.h Implementation

### ScramClient Flow

1. **`getFirstMessage()`**: Generates `n,,n=<user>,r=<clientNonce>`
2. **`getFinalMessage(serverFirst)`**: Parses server's `r=`, `s=`, `i=` values. Computes:
   - `SaltedPassword = Hi(password, salt, iterations)`
   - `ClientKey = HMAC(SaltedPassword, "Client Key")`
   - `StoredKey = H(ClientKey)`
   - `AuthMessage = clientFirstBare,serverFirst,clientFinalWithoutProof`
   - `ClientSignature = HMAC(StoredKey, AuthMessage)`
   - `ClientProof = ClientKey XOR ClientSignature`
3. **`validateServer(serverFinal)`**: Verifies `v=<serverSignature>` matches the expected value.

### ScramServer Flow

1. **`getFirstMessage(clientFirst)`**: Extracts username, looks up `DBInfo`, generates server nonce, returns `r=<combinedNonce>,s=<salt>,i=<iterations>`
2. **`getFinalMessage(clientFinal)`**: Verifies the client proof, returns `{true, "v=<serverSignature>"}` on success.

### ScramUtil::calcScram

Core proof calculation used by both client and server:
1. Compute `ClientKey`, `StoredKey`, `ServerKey` from the salted password.
2. Build `AuthMessage` from the three SCRAM messages.
3. Compute `ClientSignature = HMAC(StoredKey, AuthMessage)`.
4. Compute `ClientProof = ClientKey XOR ClientSignature`.
5. Compute `ServerSignature = HMAC(ServerKey, AuthMessage)`.

### V1 Namespace (Deprecated)

The `ThorsAnvil::Crypto::V1` namespace contains the original SCRAM API where:
- `ScramServer` takes `clientFirstMessage` in the constructor
- Uses raw strings for password/salt rather than pre-computed `DBInfo`

The new API separates concerns: `ScramMechanism` pre-computes `DBInfo`, and `ScramServer` takes a callback returning `DBInfo`.
