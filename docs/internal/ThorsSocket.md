# ThorsSocket Internal Documentation

Detailed architecture, class hierarchy, implementation patterns, and platform-specific considerations for the ThorsSocket library.

**Source:** `third/ThorsSocket/src/ThorsSocket/`

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Namespace Structure](#namespace-structure)
- [Class Hierarchy](#class-hierarchy)
- [Connection Abstraction Layer](#connection-abstraction-layer)
- [SSL/TLS Internals](#ssltls-internals)
- [SocketStreamBuffer Details](#socketstreambuffer-details)
- [Design Patterns](#design-patterns)
- [Platform Considerations](#platform-considerations)
- [Header-Only Mode](#header-only-mode)
- [Testing and Mocking](#testing-and-mocking)

---

## Architecture Overview

ThorsSocket is built around two key principles:

1. **Write synchronous-looking code that supports asynchronous operation.** Blocking I/O operations can be intercepted via a `YieldFunc` callback, allowing integration with coroutine libraries or event loops without changing application-level logic.

2. **Uniform interface across transport types.** Whether the underlying transport is a file, pipe, plain TCP socket, or TLS socket, the same `Socket` and `SocketStream` classes are used at the application layer.

### Layered Architecture

```
┌─────────────────────────────────────────────────┐
│              Application Code                    │
├─────────────────────────────────────────────────┤
│   SocketStream (std::iostream)                   │
│   SocketStreamBuffer (std::streambuf)            │
├─────────────────────────────────────────────────┤
│   Socket / Server  (public API)                  │
├─────────────────────────────────────────────────┤
│   ConnectionClient / ConnectionServer            │
│   (abstract interface)                           │
├─────────────────────────────────────────────────┤
│   SimpleFile │ Pipe │ SocketClient │ SSocketClient│
│   (concrete connection implementations)          │
├─────────────────────────────────────────────────┤
│   OS / OpenSSL                                   │
└─────────────────────────────────────────────────┘
```

---

## Namespace Structure

| Namespace | Purpose |
|---|---|
| `ThorsAnvil::ThorsSocket` | Public API classes (`Socket`, `Server`, `SocketStream`), utility types, SSL configuration types, and abstract connection interfaces. |
| `ThorsAnvil::ThorsSocket::ConnectionType` | Concrete connection implementations (`SimpleFile`, `Pipe`, `SocketClient`, `SocketServer`, `SSocketClient`, `SSocketServer`, `FileDescriptor`, `SocketStandard`, `SSocketStandard`). These are internal implementation details. |

---

## Class Hierarchy

### ConnectionBase

**Header:** `Connection.h`

Abstract base class for all connections. Non-copyable.

```cpp
class ConnectionBase {
public:
    virtual bool isConnected() const = 0;
    virtual int  socketId(Mode) const = 0;
    virtual void close() = 0;
    virtual void release() = 0;
    virtual void externalyClosed() {}
};
```

### ConnectionClient

Extends `ConnectionBase` for client-side (read/write) connections.

```cpp
class ConnectionClient: public ConnectionBase {
public:
    virtual void   tryFlushBuffer() = 0;
    virtual IOData readFromStream(char* buffer, std::size_t size) = 0;
    virtual IOData writeToStream(char const* buffer, std::size_t size) = 0;
    virtual std::string_view protocol() const = 0;
    virtual void   deferInit(YieldFunc&, YieldFunc&) {}
};
```

The `readFromStream()` and `writeToStream()` methods perform a **single** I/O operation (not looping) and return an `IOData` indicating how many bytes were transferred and the connection state. The `Socket` class is responsible for looping.

### ConnectionServer

Extends `ConnectionBase` for server-side (accept) connections.

```cpp
class ConnectionServer: public ConnectionBase {
public:
    virtual std::unique_ptr<ConnectionClient> accept(
        YieldFunc& yield, Blocking blocking, DeferAccept deferAccept) = 0;
};
```

---

## Connection Abstraction Layer

### FileDescriptor

**Header:** `ConnectionFileDescriptor.h`, **Namespace:** `ConnectionType`

Abstract intermediate class for connections using POSIX file descriptors (`read()`/`write()`). Implements `readFromStream()` and `writeToStream()` using the FDs returned by subclass-provided `getReadFD()` and `getWriteFD()`.

Error handling maps POSIX `errno` values to three categories:
- **Retry** (`EINTR`): returns `{0, true, false}` -- still open, not blocked
- **Blocked** (`EAGAIN`, `EWOULDBLOCK`, `ETIMEDOUT`): returns `{0, true, true}` -- still open, blocked
- **Closed** (`ECONNRESET`): returns `{0, false, false}` -- connection closed
- **Critical** (`EBADF`, `EFAULT`, etc.): throws `std::runtime_error`

### SimpleFile

**Namespace:** `ConnectionType`

Wraps a single file descriptor opened via `open()`. Both `getReadFD()` and `getWriteFD()` return the same fd. Protocol: `"file"`.

### Pipe

**Namespace:** `ConnectionType`

Wraps a pipe (two file descriptors: `fd[0]` for read, `fd[1]` for write). `release()` throws `std::runtime_error` (pipes cannot be released). Protocol: `"pipe"`.

### SocketStandard

**Namespace:** `ConnectionType`

Internal helper class that manages a TCP socket file descriptor. Not part of the `ConnectionBase` hierarchy. Used as a composition member by `SocketClient` and `SocketServer`.

Handles:
- Socket creation (`::socket(AF_INET, SOCK_STREAM, 0)`)
- Client-side connection (`::gethostbyname()` + `::connect()`)
- Server-side binding (`::setsockopt(SO_REUSEADDR)` + `::bind()` + `::listen()`)
- Non-blocking mode configuration

### SocketClient

**Namespace:** `ConnectionType`

Concrete connection for plain TCP clients. On Unix, inherits from `FileDescriptor` and uses POSIX `read()`/`write()`. On Windows, inherits directly from `ConnectionClient` and uses `recv()`/`send()`.

`tryFlushBuffer()` calls `shutdown()` on the socket. Protocol: `"http"`.

### SocketServer (Connection)

**Namespace:** `ConnectionType`

Concrete connection for plain TCP servers. Implements `accept()` by calling `::accept()`, using `poll()` to wait when the socket would block.

### SSocketStandard

**Namespace:** `ConnectionType`

Internal helper managing the OpenSSL `SSL*` object. Handles:
- SSL object creation and association with an existing fd
- SSL client connect handshake with retry on `SSL_ERROR_WANT_READ`/`WANT_WRITE`
- SSL server accept handshake with retry
- Certificate verification
- Deferred handshake support via `DeferAction` enum

`close()` calls `SSL_shutdown()` in a loop, then `SSL_free()`. `externalyClosed()` sets a flag to skip `SSL_shutdown()` during close.

### SSocketClient

**Namespace:** `ConnectionType`

Extends `SocketClient` with SSL encryption. Overrides `readFromStream()` and `writeToStream()` to use `SSL_read()` and `SSL_write()`.

SSL error handling maps to `IOData`:
- `SSL_ERROR_NONE` → `{0, true, false}`
- `SSL_ERROR_ZERO_RETURN` → `{0, false, false}` (peer closed)
- `SSL_ERROR_WANT_READ`/`WANT_WRITE` → `{0, true, true}` (blocked)
- `SSL_ERROR_SYSCALL`, `SSL_ERROR_SSL` → throws `std::runtime_error`

Protocol: `"https"`.

### SSocketServer (Connection)

**Namespace:** `ConnectionType`

Extends `SocketServer` for SSL. Owns an `SSLctx` and creates `SSocketClient` connections on accept.

---

## SSL/TLS Internals

### SSLUtil

Singleton that initializes the OpenSSL library. Called automatically during `SSLctx` construction.

### SSLctx Construction

Uses a variadic template constructor with a fold expression:

```cpp
template<typename... Args>
SSLctx(SSLMethodType methodType, Args&&... args) {
    // Create context
    ctx = SSL_CTX_new(methodType == Client ? TLS_client_method() : TLS_server_method());
    // Apply each configuration object
    (args.apply(ctx), ...);
}
```

### Certificate Password Handling

`CertificateInfo` uses an OpenSSL password callback. The password is securely overwritten after use by calling `std::fill` on the string's internal buffer.

### CertifcateAuthorityInfo Internals

Each sub-info (`File`, `Dir`, `Store`) can:
- Load system defaults via `SSL_CTX_set_default_verify_paths()` or similar
- Add custom paths via `SSL_CTX_load_verify_locations()` or `SSL_CTX_load_verify_file()`

### ClientCAListInfo Internals

When `verifyClientCA` is `true`, the SSL context is configured with `SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT`, requiring the client to present a valid certificate.

---

## SocketStreamBuffer Details

A `std::streambuf` subclass bridging standard stream I/O to a `Socket`. Uses internal 4KB input and output buffers.

### Key Behaviors

- **`underflow()`**: Called when the input buffer is exhausted. First attempts a non-blocking read to fill the entire buffer. If no data is available, falls back to a blocking read of at least 1 byte.
- **`xsgetn()`**: Optimized bulk read. For large reads (> half buffer size), reads directly into the destination buffer bypassing the internal buffer.
- **`overflow()`**: Flushes the output buffer when full. Uses one extra byte of reserved space in the underlying vector to avoid data loss.
- **`xsputn()`**: If data fits in remaining buffer space, copies it there. Otherwise flushes and writes directly.
- **`sync()`**: Flushes the output buffer. Returns 0 on success, -1 on failure.
- **`seekoff()`**: Only supports `std::ios_base::cur` with offset 0. Returns total bytes read or written (including buffered data).

### Buffer Resizing

```cpp
void reserveInputSize(std::size_t size);
void reserveOutputSize(std::size_t size);
```

Allow derived classes (e.g., MongoDB compressed stream buffer) to ensure the internal buffer can hold a complete protocol frame.

### Destructor

The destructor calls `overflow()` to flush remaining output data, catching and logging any exceptions.

---

## Design Patterns

### Strategy Pattern (Connection Hierarchy)

`Socket` and `Server` use `unique_ptr<ConnectionClient>` / `unique_ptr<ConnectionServer>` to hold a polymorphic connection. The concrete type is selected at construction time based on the initialization variant.

### Variant-Based Construction

`SocketInit` and `ServerInit` are `std::variant` types. The constructors use `std::visit` with a builder functor to create the appropriate connection type, avoiding manual type switching.

### Composition over Inheritance (SocketStandard, SSocketStandard)

Rather than placing socket management logic in the connection class hierarchy, `SocketStandard` and `SSocketStandard` are standalone helper classes used as composition members. This avoids diamond inheritance.

### Template Method Pattern (FileDescriptor)

`FileDescriptor` implements `readFromStream()` and `writeToStream()` using virtual `getReadFD()` and `getWriteFD()` methods overridden by concrete subclasses.

### Deferred Initialization

SSL handshake can be deferred via `DeferAccept::Yes`, allowing event-driven servers to register a socket with an event loop before completing the handshake. Triggered via `Socket::deferInit()`.

---

## Platform Considerations

| Feature | Unix | Windows |
|---|---|---|
| Socket type | `int` | `SOCKET` |
| Read/Write (TCP) | POSIX `read()`/`write()` via `FileDescriptor` | `recv()`/`send()` directly |
| Non-blocking | `O_NONBLOCK` via `fcntl()` | `ioctlsocket()` |
| Poll | `poll()` | `WSAPoll()` |
| Pipe creation | `pipe()` | `_pipe()` |
| Error retrieval | `errno` | `WSAGetLastError()` |
| Invalid FD sentinel | `-1` | `INVALID_SOCKET` |

Platform-specific functions are abstracted behind `thor*()` helper functions and `THOR_*` macros in `ConnectionUtil.h`.

---

## Header-Only Mode

When `THORS_SOCKET_HEADER_ONLY` is defined, each header includes a corresponding `.source` file. The `THORS_SOCKET_HEADER_ONLY_INCLUDE` macro controls linkage.

---

## Testing and Mocking

The codebase uses `MOCK_FUNC()` / `MOCK_TFUNC()` macros that wrap system calls and OpenSSL functions. In production builds, these resolve to real functions. In test builds, they resolve to mockable function pointers.

Key testable boundaries:
- `socket()`, `connect()`, `bind()`, `listen()`, `accept()`
- `read()`, `write()`, `close()`, `pipe()`
- `gethostbyname()`
- `SSL_new()`, `SSL_connect()`, `SSL_accept()`, `SSL_read()`, `SSL_write()`, `SSL_shutdown()`, `SSL_free()`
- `SSL_CTX_*` configuration functions

Test files are in `src/ThorsSocket/test/`.

---

## Socket I/O Loop Details

The `Socket::getMessageData()` method loops internally:

1. Call `connection->readFromStream()` for a single read operation.
2. If data was read, accumulate into the caller's buffer.
3. If blocked (`IOData::blocked == true`), invoke the registered `readYield` callback.
   - If yield returns `true`, retry immediately.
   - If yield returns `false`, fall back to `poll()` to wait for readiness.
4. If still open but zero bytes read (interrupted), retry immediately.
5. Repeat until `size` bytes are read or the connection closes.

`tryGetMessageData()` follows the same loop but returns immediately on block instead of yielding or polling.

The write methods (`putMessageData()`, `tryPutMessageData()`) are symmetric.

---

## IOData Interpretation

| dataSize | stillOpen | blocked | Meaning |
|---|---|---|---|
| N > 0 | true | false | Successfully transferred N bytes |
| 0 | false | false | Connection closed by peer |
| 0 | true | true | Would block (non-blocking mode) |
| 0 | true | false | Interrupted (e.g., EINTR), retry immediately |

---

## YieldFunc Behavior

```cpp
using YieldFunc = std::function<bool()>;
```

- Return `true`: the operation will be immediately retried (caller believes the block has been resolved, e.g., via coroutine resume).
- Return `false`: the library falls back to `poll()` to wait for the file descriptor to become ready.

Default yields return `false`.
