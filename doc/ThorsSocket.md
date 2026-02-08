# ThorsSocket Developer Documentation

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Namespace Structure](#namespace-structure)
- [Class Hierarchy](#class-hierarchy)
- [Public API Layer](#public-api-layer)
  - [Socket](#socket)
  - [Server](#server)
  - [SocketStream / BaseSocketStream](#socketstream--basesocketstream)
  - [SocketStreamBuffer](#socketstreambuffer)
- [Connection Abstraction Layer](#connection-abstraction-layer)
  - [ConnectionBase](#connectionbase)
  - [ConnectionClient](#connectionclient)
  - [ConnectionServer](#connectionserver)
  - [FileDescriptor](#filedescriptor)
  - [SimpleFile](#simplefile)
  - [Pipe](#pipe)
  - [SocketStandard](#socketstandard)
  - [SocketClient](#socketclient)
  - [SocketServer (Connection)](#socketserver-connection)
  - [SSocketStandard](#ssocketstandard)
  - [SSocketClient](#ssocketclient)
  - [SSocketServer (Connection)](#ssocketserver-connection)
- [SSL/TLS Configuration](#ssltls-configuration)
  - [SSLUtil](#sslutil)
  - [SSLctx](#sslctx)
  - [ProtocolInfo](#protocolinfo)
  - [CipherInfo](#cipherinfo)
  - [CertificateInfo](#certificateinfo)
  - [CertifcateAuthorityInfo](#certifcateauthorityinfo)
  - [ClientCAListInfo](#clientcalistinfo)
- [Utility Types and Enums](#utility-types-and-enums)
  - [IOData](#iodata)
  - [Enumerations](#enumerations)
  - [Initialization Structs](#initialization-structs)
  - [YieldFunc](#yieldfunc)
- [Design Patterns](#design-patterns)
- [Platform Considerations](#platform-considerations)
- [Header-Only Mode](#header-only-mode)
- [Testing and Mocking](#testing-and-mocking)

---

## Architecture Overview

ThorsSocket is a C++ socket library that provides a unified, type-safe abstraction over file descriptors, pipes, TCP sockets, and SSL/TLS-secured sockets. The library is designed around two key principles:

1. **Write synchronous-looking code that supports asynchronous operation.** Blocking I/O operations can be intercepted via a `YieldFunc` callback, allowing integration with coroutine libraries (e.g., Boost.Coroutine) or event loops without changing application-level logic.

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
| `ThorsAnvil::ThorsSocket::ConnectionType` | Concrete connection implementations (`SimpleFile`, `Pipe`, `SocketClient`, `SocketServer`, `SSocketClient`, `SSocketServer`, `FileDescriptor`, `SocketStandard`, `SSocketStandard`). These are internal implementation details and not intended for direct use by application code. |

---

## Public API Layer

### Socket

**Header:** `Socket.h`

The primary client-side class for reading from and writing to a connection. A `Socket` wraps a polymorphic `ConnectionClient` and provides blocking or non-blocking I/O with optional yield support.

#### Construction

```cpp
// Default (disconnected)
Socket();

// From an initialization variant (FileInfo, PipeInfo, SocketInfo, or SSocketInfo)
Socket(SocketInit const& initInfo, Blocking blocking = Blocking::Yes);

// Convenience constructors
Socket(FileInfo const& fileInfo, Blocking blocking = Blocking::Yes);
Socket(PipeInfo const& pipeInfo, Blocking blocking = Blocking::Yes);
Socket(SocketInfo const& socketInfo, Blocking blocking = Blocking::Yes);
Socket(SSocketInfo const& ssocketInfo, Blocking blocking = Blocking::Yes);
```

The constructor uses `std::visit` on a `std::variant<FileInfo, PipeInfo, SocketInfo, SSocketInfo>` to create the appropriate `ConnectionClient` subclass:
- `FileInfo` &rarr; `ConnectionType::SimpleFile`
- `PipeInfo` &rarr; `ConnectionType::Pipe`
- `SocketInfo` &rarr; `ConnectionType::SocketClient`
- `SSocketInfo` &rarr; `ConnectionType::SSocketClient`

#### Move Semantics

`Socket` is move-only. Copy construction and assignment are deleted. The moved-from socket is left in a disconnected state.

```cpp
Socket(Socket&& move) noexcept;
Socket& operator=(Socket&& move) noexcept;
void swap(Socket& other) noexcept;
```

#### Reading Data

```cpp
IOData getMessageData(void* buffer, std::size_t size);
IOData tryGetMessageData(void* buffer, std::size_t size);
```

- **`getMessageData()`** reads exactly `size` bytes. If the underlying stream would block, it invokes the registered `readYield` callback. If the yield returns `false`, it falls back to `poll()` to wait for data. This method only returns before reading `size` bytes if the connection closes.
- **`tryGetMessageData()`** reads as much as possible without blocking. Returns immediately if the operation would block, with `IOData::blocked == true`.

Both methods loop internally, accumulating partial reads into the caller's buffer.

#### Writing Data

```cpp
IOData putMessageData(void const* buffer, std::size_t size);
IOData tryPutMessageData(void const* buffer, std::size_t size);
```

Symmetric to the read operations. `putMessageData()` blocks (via yield or poll) until all data is written. `tryPutMessageData()` writes as much as possible without blocking.

#### State Management

```cpp
bool isConnected() const;       // true if connection is valid and open
int  socketId(Mode rw) const;   // underlying file descriptor
int  socketId() const;          // shorthand for socketId(Mode::Read)
void close();                   // close the connection
void release();                 // release ownership of the fd without closing
void externalyClosed();         // notify the connection it was closed externally
```

- `isConnected()` checks both that the internal `unique_ptr<ConnectionClient>` is non-null and that the connection reports itself as connected.
- `close()`, `release()`, `socketId()` all throw `std::runtime_error` if the socket is not connected.
- `externalyClosed()` is safe to call on a null connection (no-op).

#### Asynchronous Support

```cpp
void setReadYield(YieldFunc&& yield);
void setWriteYield(YieldFunc&& yield);
void deferInit();
std::string_view protocol();
```

- `setReadYield()` / `setWriteYield()` register callbacks invoked when a blocking read/write would occur. Default yields return `false` (meaning: fall through to `poll()`).
- `deferInit()` triggers deferred SSL handshake operations (connect or accept), passing the registered yield functions. Only meaningful for `SSocketClient` connections.
- `protocol()` returns a protocol string (`"file"`, `"pipe"`, `"http"`, `"https"`) from the underlying connection.

#### Flush

```cpp
void tryFlushBuffer();
```

Delegates to the connection's `tryFlushBuffer()`. For TCP sockets this calls `shutdown()` on the socket. For file descriptors and pipes this is a no-op.

---

### Server

**Header:** `Server.h`

Listens on a port for incoming TCP connections and produces `Socket` objects.

#### Construction

```cpp
Server(ServerInit&& serverInit, Blocking blocking = Blocking::Yes);

// Convenience constructors
Server(ServerInfo&& socketInit, Blocking blocking = Blocking::Yes);
Server(SServerInfo&& secureInit, Blocking blocking = Blocking::Yes);
Server(int port, Blocking blocking = Blocking::Yes);
Server(int port, SSLctx&& ctx, Blocking blocking = Blocking::Yes);
```

`ServerInit` is `std::variant<ServerInfo, SServerInfo>`. The constructor creates either a `ConnectionType::SocketServer` or `ConnectionType::SSocketServer` based on the variant.

The underlying server socket is created, bound to `INADDR_ANY` on the given port with `SO_REUSEADDR`, and begins listening with a backlog of 5.

#### Move Semantics

Move-only, same pattern as `Socket`.

```cpp
Server(Server&& move) noexcept;
Server& operator=(Server&& move) noexcept;
void swap(Server& other) noexcept;
```

#### Accepting Connections

```cpp
Socket accept(Blocking blocking = Blocking::Yes, DeferAccept deferAccept = DeferAccept::No);
```

Blocks (or yields) until a client connects, then returns a `Socket` wrapping the accepted connection. The `blocking` parameter controls whether the *returned socket* is blocking or non-blocking.

For SSL servers, the accepted socket will be an `SSocketClient`. If `deferAccept` is `DeferAccept::Yes`, the SSL handshake is deferred until `Socket::deferInit()` is called.

#### State and Yield

```cpp
bool isConnected() const;
int  socketId(Mode rw) const;
int  socketId() const;         // shorthand for socketId(Mode::Read)
void close();
void release();
void setYield(YieldFunc&& yieldFunc);
```

`setYield()` registers a callback invoked when `accept()` would block on a non-blocking server.

---

### SocketStream / BaseSocketStream

**Header:** `SocketStream.h`, `SocketStream.tpp`

A `std::iostream` subclass that wraps a `Socket` via a `SocketStreamBuffer`, allowing standard C++ stream operations (`<<`, `>>`, `getline()`, etc.) over any connection type.

```cpp
template<typename Buffer = SocketStreamBuffer>
class BaseSocketStream: public std::iostream;

using SocketStream = BaseSocketStream<SocketStreamBuffer>;
```

#### Construction

```cpp
BaseSocketStream();                              // disconnected
BaseSocketStream(Socket&& socket);               // takes ownership of socket
BaseSocketStream(PipeInfo const& info);           // creates socket internally
BaseSocketStream(FileInfo const& info);
BaseSocketStream(SocketInfo const& info);
BaseSocketStream(SSocketInfo const& info);
```

When constructed with an info struct directly, the internal buffer creates the socket with `Blocking::No`.

#### Move Semantics

Move-only. On move, the internal `rdbuf()` pointer is re-set to the new buffer.

#### Key Methods

```cpp
void close();                            // close the underlying socket
Socket& getSocket();                     // access the underlying socket
Socket const& getSocket() const;
operator bool() const;                   // true if socket is connected
```

The `operator bool()` allows idiomatic stream-style checks:
```cpp
SocketStream stream(std::move(socket));
if (stream) {
    stream << "Hello";
}
```

---

### SocketStreamBuffer

**Header:** `SocketStreamBuffer.h`

A `std::streambuf` subclass that bridges standard stream I/O to a `Socket`. Uses internal 4KB input and output buffers.

#### Key Behaviors

- **`underflow()`**: Called when the input buffer is exhausted. First attempts a non-blocking read (`tryGetMessageData`) to fill the entire buffer. If no data is available, falls back to a blocking read of at least 1 byte (`getMessageData`).
- **`xsgetn()`**: Optimized bulk read. For large reads (> half buffer size), reads directly into the destination buffer bypassing the internal buffer.
- **`overflow()`**: Flushes the output buffer to the socket when full. Uses one extra byte of reserved space in the underlying vector to avoid data loss.
- **`xsputn()`**: If data fits in the remaining buffer space, copies it there. Otherwise flushes and writes directly to the stream.
- **`sync()`**: Flushes the output buffer. Returns 0 on success, -1 on failure.
- **`seekoff()`**: Only supports `std::ios_base::cur` direction with offset 0. Returns the total bytes read or written (including buffered data). Useful for tracking stream position.

#### Buffer Resizing

```cpp
void reserveInputSize(std::size_t size);
void reserveOutputSize(std::size_t size);
```

These allow derived classes (e.g., a MongoDB compressed stream buffer) to ensure the internal buffer is large enough to hold a complete protocol frame before processing.

#### Destructor

The destructor calls `overflow()` to flush any remaining output data, catching and logging any exceptions (destructors must not throw).

---

## Connection Abstraction Layer

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
    virtual void externalyClosed() {}    // optional override
};
```

### ConnectionClient

**Header:** `Connection.h`

Extends `ConnectionBase` for client-side (read/write) connections.

```cpp
class ConnectionClient: public ConnectionBase {
public:
    virtual void   tryFlushBuffer() = 0;
    virtual IOData readFromStream(char* buffer, std::size_t size) = 0;
    virtual IOData writeToStream(char const* buffer, std::size_t size) = 0;
    virtual std::string_view protocol() const = 0;
    virtual void   deferInit(YieldFunc&, YieldFunc&) {}   // optional
};
```

The `readFromStream()` and `writeToStream()` methods perform a single I/O operation (not looping) and return an `IOData` indicating how many bytes were transferred and the connection state. The `Socket` class is responsible for looping.

### ConnectionServer

**Header:** `Connection.h`

Extends `ConnectionBase` for server-side (accept) connections.

```cpp
class ConnectionServer: public ConnectionBase {
public:
    virtual std::unique_ptr<ConnectionClient> accept(
        YieldFunc& yield,
        Blocking blocking = Blocking::Yes,
        DeferAccept deferAccept = DeferAccept::No
    ) = 0;
};
```

### FileDescriptor

**Header:** `ConnectionFileDescriptor.h`  
**Namespace:** `ConnectionType`

Abstract intermediate class for connections that use POSIX file descriptors (`read()`/`write()`). Implements `readFromStream()` and `writeToStream()` using the FDs returned by subclass-provided `getReadFD()` and `getWriteFD()`.

Error handling maps POSIX `errno` values to three categories:
- **Retry** (`EINTR`): returns `{0, true, false}` -- still open, not blocked
- **Blocked** (`EAGAIN`, `EWOULDBLOCK`, `ETIMEDOUT`): returns `{0, true, true}` -- still open, blocked
- **Closed** (`ECONNRESET`): returns `{0, false, false}` -- connection closed
- **Critical** (`EBADF`, `EFAULT`, etc.): throws `std::runtime_error`
- **Unknown**: throws `std::runtime_error` with a warning

`tryFlushBuffer()` is a no-op for file descriptor based connections.

### SimpleFile

**Header:** `ConnectionSimpleFile.h`  
**Namespace:** `ConnectionType`

Wraps a single file descriptor opened via `open()`. Constructed from a `FileInfo` struct containing a filename and `FileMode` (Read, WriteAppend, WriteTruncate).

```cpp
class SimpleFile: public FileDescriptor {
public:
    SimpleFile(FileInfo const& fileInfo, Blocking blocking);
    SimpleFile(int fd);   // wrap existing fd
};
```

Both `getReadFD()` and `getWriteFD()` return the same fd. `release()` sets the fd to -1 without closing. Protocol: `"file"`.

### Pipe

**Header:** `ConnectionPipe.h`  
**Namespace:** `ConnectionType`

Wraps a pipe (two file descriptors: `fd[0]` for read, `fd[1]` for write).

```cpp
class Pipe: public FileDescriptor {
public:
    Pipe(PipeInfo const& pipeInfo, Blocking blocking);
    Pipe(int fd[]);   // wrap existing pipe fds
};
```

`socketId(Mode::Read)` returns `fd[0]`, `socketId(Mode::Write)` returns `fd[1]`. `release()` throws `std::runtime_error` (pipes cannot be released). Protocol: `"pipe"`.

### SocketStandard

**Header:** `ConnectionSocket.h`  
**Namespace:** `ConnectionType`

Internal helper class that manages a TCP socket file descriptor. Not part of the `ConnectionBase` hierarchy. Used as a composition member by `SocketClient` and `SocketServer`.

Handles:
- Socket creation (`::socket(AF_INET, SOCK_STREAM, 0)`)
- Client-side connection (`::gethostbyname()` + `::connect()`)
- Server-side binding (`::setsockopt(SO_REUSEADDR)` + `::bind()` + `::listen()`)
- Non-blocking mode configuration

```cpp
class SocketStandard {
public:
    SocketStandard(ServerInfo const& socketInfo, Blocking blocking);
    SocketStandard(SocketInfo const& socketInfo, Blocking blocking);
    SocketStandard(OpenSocketInfo const& socketInfo, Blocking blocking);

    bool isConnected() const;
    int  socketId(Mode rw) const;
    void close();
    void release();
    int  getFD() const;
};
```

### SocketClient

**Header:** `ConnectionSocket.h`  
**Namespace:** `ConnectionType`

Concrete connection for plain TCP clients. On Unix, inherits from `FileDescriptor` and uses POSIX `read()`/`write()`. On Windows, inherits directly from `ConnectionClient` and uses `recv()`/`send()`.

```cpp
class SocketClient: public FileDescriptor {   // Unix
    SocketStandard socketInfo;
public:
    SocketClient(SocketInfo const& socketInfo, Blocking blocking);
    SocketClient(SocketServer&, OpenSocketInfo const& socketInfo, Blocking blocking);
};
```

`tryFlushBuffer()` calls `shutdown()` on the socket, signaling the write-end is done. Protocol: `"http"`.

### SocketServer (Connection)

**Header:** `ConnectionSocket.h`  
**Namespace:** `ConnectionType`

Concrete connection for plain TCP servers. Implements `accept()` by calling `::accept()`, using `poll()` to wait when the socket would block.

```cpp
class SocketServer: public ConnectionServer {
    SocketStandard socketInfo;
public:
    SocketServer(ServerInfo&& socketInfo, Blocking blocking);
    virtual std::unique_ptr<ConnectionClient> accept(
        YieldFunc& yield, Blocking blocking, DeferAccept deferAccept) override;
protected:
    int  acceptSocket(YieldFunc& yield);
    void waitForFileDescriptor(int fd);
    bool wouldBlock(int errorCode);
};
```

The `acceptSocket()` method is protected so `SSocketServer` can reuse it.

### SSocketStandard

**Header:** `ConnectionSSocket.h`  
**Namespace:** `ConnectionType`

Internal helper managing the OpenSSL `SSL*` object for a secure connection. Analogous to `SocketStandard` but for the SSL layer.

Handles:
- SSL object creation and association with an existing fd (`SSL_new`, `SSL_set_fd`)
- SSL client connect handshake (`SSL_connect`) with retry on `SSL_ERROR_WANT_READ`/`WANT_WRITE`
- SSL server accept handshake (`SSL_accept`) with retry
- Certificate verification (`SSL_get1_peer_certificate`, `SSL_get_verify_result`)
- Deferred handshake support via `DeferAction` enum

```cpp
enum class DeferAction { None, Connect, Accept };

class SSocketStandard {
    SSL*        ssl;
    bool        connectionFailed;
    DeferAction deferAction;
public:
    SSocketStandard(SSocketInfo const& socketInfo, int fd);
    SSocketStandard(OpenSSocketInfo const& socketInfo, int fd);
    ~SSocketStandard();

    bool isConnected() const;
    void close();
    void externalyClosed();
    SSL* getSSL() const;
    void checkConnectionOK(int errorCode);
    void deferInit(YieldFunc& rYield, YieldFunc& wYield);
};
```

- `close()` calls `SSL_shutdown()` in a loop (handling `SSL_ERROR_WANT_READ`), then `SSL_free()`.
- `externalyClosed()` sets a flag to skip `SSL_shutdown()` during close (avoiding errors on an already-dead connection).
- `checkConnectionOK()` tracks whether a fatal SSL error (`SSL_ERROR_SYSCALL` or `SSL_ERROR_SSL`) has occurred.

### SSocketClient

**Header:** `ConnectionSSocket.h`  
**Namespace:** `ConnectionType`

Extends `SocketClient` with SSL encryption. Overrides `readFromStream()` and `writeToStream()` to use `SSL_read()` and `SSL_write()`.

```cpp
class SSocketClient: public SocketClient {
    SSocketStandard secureSocketInfo;
public:
    SSocketClient(SSocketInfo const& socketInfo, Blocking blocking);
    SSocketClient(SSocketServer&, OpenSSocketInfo const& socketInfo, Blocking blocking);

    IOData readFromStream(char* buffer, std::size_t size) override;
    IOData writeToStream(char const* buffer, std::size_t size) override;
    void   deferInit(YieldFunc& rYield, YieldFunc& wYield) override;
};
```

SSL error handling maps to `IOData`:
- `SSL_ERROR_NONE` &rarr; `{0, true, false}`
- `SSL_ERROR_ZERO_RETURN` &rarr; `{0, false, false}` (peer closed)
- `SSL_ERROR_WANT_READ` (on read) / `SSL_ERROR_WANT_WRITE` (on write) &rarr; `{0, true, true}` (blocked)
- `SSL_ERROR_SYSCALL`, `SSL_ERROR_SSL` &rarr; throws `std::runtime_error`

Protocol: `"https"`.

### SSocketServer (Connection)

**Header:** `ConnectionSSocket.h`  
**Namespace:** `ConnectionType`

Extends `SocketServer` for SSL. Owns an `SSLctx` and creates `SSocketClient` connections on accept.

```cpp
class SSocketServer: public SocketServer {
    SSLctx ctx;
public:
    SSocketServer(SServerInfo&& socketInfo, Blocking blocking);
    std::unique_ptr<ConnectionClient> accept(
        YieldFunc& yield, Blocking blocking, DeferAccept deferAccept) override;
};
```

---

## SSL/TLS Configuration

All SSL configuration types live in `ThorsAnvil::ThorsSocket` and are defined in `SecureSocketUtil.h`.

### SSLUtil

Singleton that initializes the OpenSSL library. Called automatically during `SSLctx` construction.

```cpp
class SSLUtil {
    SSLUtil();   // calls SSL_load_error_strings(), SSL_library_init()
public:
    static SSLUtil& getInstance();
};
```

### SSLctx

Wraps an `SSL_CTX*` object. Variadic template constructor accepts any combination of configuration objects.

```cpp
template<typename... Args>
SSLctx(SSLMethodType methodType, Args&&... args);
~SSLctx();
```

`SSLMethodType::Client` uses `TLS_client_method()`, `SSLMethodType::Server` uses `TLS_server_method()`. After creating the context, each argument's `apply(SSL_CTX*)` method is called via a fold expression: `(args.apply(ctx), ...)`.

Move-only. On move, the source `ctx` is set to `nullptr`.

### ProtocolInfo

Constrains the TLS protocol version range.

```cpp
struct ProtocolInfo {
    ProtocolInfo();                                          // TLS 1.2 - 1.3
    ProtocolInfo(Protocol minProtocol, Protocol maxProtocol);
    void apply(SSL_CTX* ctx) const;
    void apply(SSL* ssl) const;
};

enum Protocol { TLS_1_0, TLS_1_1, TLS_1_2, TLS_1_3 };
```

### CipherInfo

Specifies allowed cipher lists and cipher suites.

```cpp
struct CipherInfo {
    std::string cipherList;    // For TLS 1.2 and below
    std::string cipherSuite;   // For TLS 1.3
    void apply(SSL_CTX* ctx) const;
    void apply(SSL* ssl) const;
};
```

Defaults are secure modern ciphers (ECDHE-based with AES-GCM and CHACHA20-POLY1305).

### CertificateInfo

Specifies certificate and private key files for the connection.

```cpp
struct CertificateInfo {
    using GetPasswordFunc = std::function<std::string(int)>;

    CertificateInfo(std::string const& certificateFileName,
                    std::string const& keyFileName);
    CertificateInfo(std::string const& certificateFileName,
                    std::string const& keyFileName,
                    GetPasswordFunc&& getPassword);

    void apply(SSL_CTX* ctx) const;
    void apply(SSL* ssl) const;
};
```

Both filenames must be non-empty. Certificate files are expected in PEM format. The optional `GetPasswordFunc` receives the `rwflag` (0=decryption, 1=encryption) and should return the password string. The password is securely overwritten after use.

### CertifcateAuthorityInfo

Configures certificate authority (CA) trust stores for verifying peer certificates.

```cpp
struct CertifcateAuthorityInfo {
    CertifcateAuthorityDataInfo<File>   file;   // CA files
    CertifcateAuthorityDataInfo<Dir>    dir;    // CA directories
    CertifcateAuthorityDataInfo<Store>  store;  // CA stores

    void apply(SSL_CTX* ctx) const;
};
```

Each sub-info has:
```cpp
template<AuthorityType A>
struct CertifcateAuthorityDataInfo {
    bool                        loadDefault = false;   // load system defaults
    std::vector<std::string>    items;                 // additional paths

    void apply(SSL_CTX* ctx) const;
};
```

`AuthorityType` is `enum { File, Dir, Store }`.

### ClientCAListInfo

Configures the list of acceptable client certificate authorities (sent to clients during TLS handshake on servers).

```cpp
struct ClientCAListInfo {
    bool                           verifyClientCA = false;
    ClientCAListDataInfo<File>     file;
    ClientCAListDataInfo<Dir>      dir;
    ClientCAListDataInfo<Store>    store;

    void apply(SSL_CTX* ctx) const;
    void apply(SSL* ssl) const;
};
```

When `verifyClientCA` is `true`, the SSL context is configured with `SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT`, requiring the client to present a valid certificate.

---

## Utility Types and Enums

### IOData

**Header:** `SocketUtil.h`

Returned by all read/write operations to communicate the result of a single I/O attempt.

```cpp
struct IOData {
    std::size_t dataSize;    // bytes transferred in this operation
    bool        stillOpen;   // false if connection is closed
    bool        blocked;     // true if operation would block
};
```

Interpretation guide:
| dataSize | stillOpen | blocked | Meaning |
|---|---|---|---|
| N > 0 | true | false | Successfully transferred N bytes |
| 0 | false | false | Connection closed by peer |
| 0 | true | true | Would block (non-blocking mode) |
| 0 | true | false | Interrupted (e.g., EINTR), retry immediately |

### Enumerations

**Header:** `SocketUtil.h`

```cpp
enum class FileMode    { Read, WriteAppend, WriteTruncate };
enum class Blocking    { No, Yes };
enum class Mode        { Read, Write };
enum class DeferAccept { No, Yes };
```

- `FileMode`: Determines how `SimpleFile` opens a file.
- `Blocking`: Controls whether I/O operations and accept are blocking or non-blocking.
- `Mode`: Distinguishes read vs write when querying `socketId()` (relevant for `Pipe` which has separate read/write fds).
- `DeferAccept`: When `Yes`, SSL handshake is deferred until `Socket::deferInit()` is called. Useful for event-driven servers that need to register the socket before completing the handshake.

### Initialization Structs

**Header:** `SocketUtil.h`

```cpp
struct FileInfo {
    std::string_view fileName;
    FileMode         mode;
};

struct PipeInfo {};   // No configuration needed

struct SocketInfo {
    std::string_view host;
    int              port;
};

struct ServerInfo {
    int port;
};

struct OpenSocketInfo {
    SOCKET_TYPE fd;   // Wrap an existing file descriptor
};

struct SSocketInfo: public SocketInfo {
    SSLctx const& ctx;
    DeferAccept    defer;
};

struct SServerInfo: public ServerInfo {
    SSLctx ctx;   // Owns the context (moved in)
};

struct OpenSSocketInfo: public OpenSocketInfo {
    SSLctx const& ctx;
    DeferAccept    defer;
};
```

Note: `SServerInfo` *owns* its `SSLctx` (it is moved in), while `SSocketInfo` and `OpenSSocketInfo` hold a const reference to an externally-owned `SSLctx`.

### YieldFunc

```cpp
using YieldFunc = std::function<bool()>;
```

A callback invoked when an I/O operation would block.
- Return `true`: the operation will be immediately retried (caller believes the block has been resolved, e.g., via coroutine resume).
- Return `false`: the library falls back to `poll()` to wait for the file descriptor to become ready.

---

## Design Patterns

### Strategy Pattern (Connection Hierarchy)

The `Socket` and `Server` classes use a `unique_ptr<ConnectionClient>` / `unique_ptr<ConnectionServer>` to hold a polymorphic connection. The concrete type is selected at construction time based on the initialization variant. This allows all transport types to share a single public interface.

### Variant-Based Construction

`SocketInit` and `ServerInit` are `std::variant` types. The constructors use `std::visit` with a builder functor to create the appropriate connection type, avoiding manual type switching.

### Composition over Inheritance (SocketStandard, SSocketStandard)

Rather than placing socket management logic in the connection class hierarchy, `SocketStandard` and `SSocketStandard` are standalone helper classes used as composition members. This avoids diamond inheritance and keeps the connection hierarchy focused on the `ConnectionClient`/`ConnectionServer` interface.

### Template Method Pattern (FileDescriptor)

`FileDescriptor` implements `readFromStream()` and `writeToStream()` using virtual `getReadFD()` and `getWriteFD()` methods overridden by concrete subclasses. This avoids duplicating I/O and error-handling code.

### Deferred Initialization

SSL handshake can be deferred via `DeferAccept::Yes`, allowing event-driven servers to register a socket with an event loop before completing the handshake. The handshake is triggered manually via `Socket::deferInit()`, which passes the registered yield functions through to the SSL layer.

---

## Platform Considerations

The library supports both Unix and Windows via conditional compilation (`__WINNT__`).

| Feature | Unix | Windows |
|---|---|---|
| Socket type | `int` | `SOCKET` |
| Read/Write (TCP) | POSIX `read()`/`write()` via `FileDescriptor` | `recv()`/`send()` directly |
| Non-blocking | `O_NONBLOCK` via `fcntl()` | `ioctlsocket()` |
| Poll | `poll()` | `WSAPoll()` |
| Pipe creation | `pipe()` | Emulated via `_pipe()` |
| Error retrieval | `errno` | `WSAGetLastError()` |
| Invalid FD sentinel | `-1` | `INVALID_SOCKET` |

Platform-specific functions are abstracted behind `thor*()` helper functions and `THOR_*` macros defined in `ConnectionUtil.h`.

---

## Header-Only Mode

When `THORS_SOCKET_HEADER_ONLY` is defined, each header includes a corresponding `.source` file containing the implementation. The `THORS_SOCKET_HEADER_ONLY_INCLUDE` macro controls whether functions have external linkage or are inlined.

This allows the library to be used either as a traditional compiled library or as a header-only library by defining the appropriate preprocessor symbol.

---

## Testing and Mocking

The codebase uses a `MOCK_FUNC()` / `MOCK_TFUNC()` macro system that wraps system calls and OpenSSL functions. In production builds, these resolve directly to the real functions. In test builds, they resolve to mockable function pointers, allowing unit tests to inject failures at any system call boundary.

Test files are located in `src/ThorsSocket/test/`. The `MockHeaderInclude.h` file provides the mock infrastructure, and `ConnectionTest.h` provides common test utilities.

Key testable boundaries include:
- `socket()`, `connect()`, `bind()`, `listen()`, `accept()`
- `read()`, `write()`, `close()`, `pipe()`
- `gethostbyname()`
- `SSL_new()`, `SSL_connect()`, `SSL_accept()`, `SSL_read()`, `SSL_write()`, `SSL_shutdown()`, `SSL_free()`
- `SSL_CTX_*` configuration functions
- `thorSetSocketNonBlocking()`, `thorSetFDNonBlocking()`, `thorCloseSocket()`
