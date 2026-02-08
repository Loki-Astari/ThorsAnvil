[Home](../index.html) | [API Documentation](../ThorsSocket.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsSocket Internal Documentation

Detailed architecture, class hierarchy, and member-level documentation for the ThorsSocket library.

**Source:** `third/ThorsSocket/src/ThorsSocket/`

---

## Table of Contents

- [File Map](#file-map)
- [Architecture Overview](#architecture-overview)
- [Namespace Structure](#namespace-structure)
- [Enumerations and Type Aliases](#enumerations-and-type-aliases)
- [Initialization Structs](#initialization-structs)
- [IOData Struct](#iodata-struct)
- [Socket Class](#socket-class)
- [Server Class](#server-class)
- [BaseSocketStream\<Buffer\> Class](#basesocketstreambuffer-class)
- [SocketStreamBuffer Class](#socketstreambuffer-class)
- [ConnectionBase Class](#connectionbase-class)
- [ConnectionClient Class](#connectionclient-class)
- [ConnectionServer Class](#connectionserver-class)
- [FileDescriptor Class](#filedescriptor-class)
- [SimpleFile Class](#simplefile-class)
- [Pipe Class](#pipe-class)
- [SocketStandard Class](#socketstandard-class)
- [SocketClient Class](#socketclient-class)
- [SocketServer Class](#socketserver-class)
- [SSocketStandard Class](#ssocketstandard-class)
- [SSocketClient Class](#ssocketclient-class)
- [SSocketServer Class](#ssocketserver-class)
- [SSLUtil Class](#sslutil-class)
- [SSLctx Class](#sslctx-class)
- [ProtocolInfo Struct](#protocolinfo-struct)
- [CipherInfo Struct](#cipherinfo-struct)
- [CertificateInfo Struct](#certificateinfo-struct)
- [CertifcateAuthorityInfo Struct](#certifcateauthorityinfo-struct)
- [ClientCAListInfo Struct](#clientcalistinfo-struct)
- [Platform Abstraction (ConnectionUtil)](#platform-abstraction-connectionutil)
- [Design Patterns](#design-patterns)
- [Header-Only Mode](#header-only-mode)
- [Testing and Mocking](#testing-and-mocking)

---

## File Map

| File | Namespace | Contents |
|------|-----------|----------|
| `SocketUtil.h` | `ThorsAnvil::ThorsSocket` | `IOData`, enums, initialization structs, `YieldFunc` |
| `Socket.h` / `.cpp` | `ThorsAnvil::ThorsSocket` | `Socket` class |
| `Server.h` / `.cpp` | `ThorsAnvil::ThorsSocket` | `Server` class |
| `SocketStream.h` / `.tpp` | `ThorsAnvil::ThorsSocket` | `BaseSocketStream<Buffer>`, `SocketStream` alias |
| `SocketStreamBuffer.h` / `.cpp` | `ThorsAnvil::ThorsSocket` | `SocketStreamBuffer` class |
| `Connection.h` | `ThorsAnvil::ThorsSocket` | `ConnectionBase`, `ConnectionClient`, `ConnectionServer` |
| `ConnectionFileDescriptor.h` / `.cpp` | `ThorsAnvil::ThorsSocket::ConnectionType` | `FileDescriptor` abstract class |
| `ConnectionSimpleFile.h` / `.cpp` | `ThorsAnvil::ThorsSocket::ConnectionType` | `SimpleFile` class |
| `ConnectionPipe.h` / `.cpp` | `ThorsAnvil::ThorsSocket::ConnectionType` | `Pipe` class |
| `ConnectionSocket.h` / `.cpp` | `ThorsAnvil::ThorsSocket::ConnectionType` | `SocketStandard`, `SocketClient`, `SocketServer` |
| `ConnectionSSocket.h` / `.cpp` | `ThorsAnvil::ThorsSocket::ConnectionType` | `SSocketStandard`, `SSocketClient`, `SSocketServer` |
| `SecureSocketUtil.h` / `.cpp` | `ThorsAnvil::ThorsSocket` | `SSLUtil`, `SSLctx`, `ProtocolInfo`, `CipherInfo`, `CertificateInfo`, `CertifcateAuthorityInfo`, `ClientCAListInfo` |
| `ConnectionUtil.h` / `.cpp` | (global / macros) | Platform abstraction functions and macros |

---

## Architecture Overview

```
┌─────────────────────────────────────────────────┐
│              Application Code                    │
├─────────────────────────────────────────────────┤
│   BaseSocketStream (std::iostream)               │
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
|-----------|---------|
| `ThorsAnvil::ThorsSocket` | Public API: `Socket`, `Server`, `BaseSocketStream`, `SocketStreamBuffer`, utility types, SSL configuration types, and abstract connection interfaces. |
| `ThorsAnvil::ThorsSocket::ConnectionType` | Concrete connection implementations. Internal -- not intended for direct use by application code. |

---

## Enumerations and Type Aliases

**Header:** `SocketUtil.h`

```cpp
enum class FileMode    { Read, WriteAppend, WriteTruncate };
enum class Blocking    { No, Yes };
enum class Mode        { Read, Write };
enum class DeferAccept { No, Yes };

using YieldFunc  = std::function<bool()>;
using SocketInit = std::variant<FileInfo, PipeInfo, SocketInfo, SSocketInfo>;
using ServerInit = std::variant<ServerInfo, SServerInfo>;
```

**Header:** `ConnectionSSocket.h`

```cpp
enum class DeferAction { None, Connect, Accept };   // internal to SSocketStandard
```

**Header:** `SecureSocketUtil.h`

```cpp
enum class SSLMethodType { Client, Server };
enum Protocol            { TLS_1_0, TLS_1_1, TLS_1_2, TLS_1_3 };
enum AuthorityType       { File, Dir, Store };
```

**Header:** `Socket.h`

```cpp
enum class TestMarker { True };   // Tag type for test-only constructor
```

---

## Initialization Structs

**Header:** `SocketUtil.h`

| Struct | Fields | Description |
|--------|--------|-------------|
| `FileInfo` | `std::string_view fileName`, `FileMode mode` | Open a local file |
| `PipeInfo` | (none) | Create a pipe pair |
| `SocketInfo` | `std::string_view host`, `int port` | TCP client connection |
| `ServerInfo` | `int port` | TCP server listener |
| `OpenSocketInfo` | `SOCKET_TYPE fd` | Wrap an existing file descriptor |
| `SSocketInfo` | inherits `SocketInfo` + `SSLctx const& ctx`, `DeferAccept defer` | TLS client connection. Holds a **const reference** to an externally-owned `SSLctx`. |
| `SServerInfo` | inherits `ServerInfo` + `SSLctx ctx` | TLS server listener. **Owns** the `SSLctx` (moved in). |
| `OpenSSocketInfo` | inherits `OpenSocketInfo` + `SSLctx const& ctx`, `DeferAccept defer` | Wrap an existing fd as TLS. Holds a **const reference** to `SSLctx`. |

---

## IOData Struct

**Header:** `SocketUtil.h`

```cpp
struct IOData {
    std::size_t dataSize;   // bytes transferred in this operation
    bool        stillOpen;  // false if connection is closed
    bool        blocked;    // true if operation would block
};
```

| dataSize | stillOpen | blocked | Meaning |
|----------|-----------|---------|---------|
| N > 0 | true | false | Successfully transferred N bytes |
| 0 | false | false | Connection closed by peer |
| 0 | true | true | Would block (non-blocking mode) |
| 0 | true | false | Interrupted (e.g., EINTR), retry immediately |

---

## Socket Class

**Header:** `Socket.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

The primary client-side class for reading from and writing to a connection.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `connection` | `std::unique_ptr<ConnectionClient>` | Polymorphic connection. Null when disconnected. |
| `readYield` | `YieldFunc` | Callback invoked when a read would block. Default returns `false`. |
| `writeYield` | `YieldFunc` | Callback invoked when a write would block. Default returns `false`. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `Socket()` | Default -- creates a disconnected socket. |
| `Socket(SocketInit const& initInfo, Blocking blocking = Blocking::Yes)` | Creates the appropriate `ConnectionClient` subclass via `std::visit` on the variant. |
| `Socket(FileInfo const&, Blocking)` | Convenience -- delegates to `SocketInit` constructor. |
| `Socket(PipeInfo const&, Blocking)` | Convenience -- delegates to `SocketInit` constructor. |
| `Socket(SocketInfo const&, Blocking)` | Convenience -- delegates to `SocketInit` constructor. |
| `Socket(SSocketInfo const&, Blocking)` | Convenience -- delegates to `SocketInit` constructor. |
| `Socket(std::unique_ptr<ConnectionClient>&&)` | Private. Used by `Server::accept()` to wrap an accepted connection. `friend class Server`. |
| `Socket(TestMarker, T const& socket)` | Template test constructor. Creates a connection from a test mock type via `std::make_unique<typename T::Connection>(socket)`. |

### Move Semantics

| Signature | Description |
|-----------|-------------|
| `Socket(Socket&& move) noexcept` | Move constructor. Moved-from socket is disconnected. |
| `Socket& operator=(Socket&& move) noexcept` | Move assignment. |
| `void swap(Socket& other) noexcept` | Swaps all members. |
| `Socket(Socket const&) = delete` | Copy deleted. |
| `Socket& operator=(Socket const&) = delete` | Copy deleted. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if `connection` is non-null and `connection->isConnected()`. |
| `socketId(Mode rw) const` | `int` | Underlying file descriptor for the given mode. Throws if not connected. |
| `socketId() const` | `int` | Shorthand for `socketId(Mode::Read)`. |
| `getMessageData(void* buffer, std::size_t size)` | `IOData` | Reads exactly `size` bytes. Blocks via yield or poll. Delegates to `getMessageDataFromStream(b, size, true)`. |
| `tryGetMessageData(void* buffer, std::size_t size)` | `IOData` | Reads as much as possible without blocking. Delegates to `getMessageDataFromStream(b, size, false)`. |
| `putMessageData(void const* buffer, std::size_t size)` | `IOData` | Writes all data. Blocks via yield or poll. Delegates to `putMessageDataToStream(b, size, true)`. |
| `tryPutMessageData(void const* buffer, std::size_t size)` | `IOData` | Writes as much as possible without blocking. Delegates to `putMessageDataToStream(b, size, false)`. |
| `tryFlushBuffer()` | `void` | Delegates to `connection->tryFlushBuffer()`. For TCP sockets this calls `shutdown()`. |
| `close()` | `void` | Delegates to `connection->close()`. Throws if not connected. |
| `release()` | `void` | Releases ownership of the fd without closing. Throws if not connected. |
| `externalyClosed()` | `void` | Notifies connection it was closed externally. Safe to call on null connection (no-op). |
| `setReadYield(YieldFunc&& yield)` | `void` | Registers the read yield callback. |
| `setWriteYield(YieldFunc&& yield)` | `void` | Registers the write yield callback. |
| `deferInit()` | `void` | Triggers deferred SSL handshake. Calls `connection->deferInit(readYield, writeYield)`. |
| `protocol()` | `std::string_view` | Returns protocol string from the connection (`"file"`, `"pipe"`, `"http"`, `"https"`). |

### Private Methods

| Method | Return | Description |
|--------|--------|-------------|
| `getMessageDataFromStream(void* b, std::size_t size, bool waitWhenBlocking)` | `IOData` | Core read loop. Calls `connection->readFromStream()` repeatedly, accumulating bytes. On block: if `waitWhenBlocking`, invokes `readYield` (if returns true, retry; if false, calls `waitForInput()` which uses `poll()`). Stops when `size` bytes read or connection closes. |
| `putMessageDataToStream(void const* b, std::size_t size, bool waitWhenBlocking)` | `IOData` | Core write loop. Symmetric to read. On block: invokes `writeYield` then `waitForOutput()`. |
| `waitForInput()` | `void` | Calls `waitForFileDescriptor(fd, POLLIN)` on the read fd. |
| `waitForOutput()` | `void` | Calls `waitForFileDescriptor(fd, POLLOUT)` on the write fd. |
| `waitForFileDescriptor(int fd, short flag)` | `void` | Blocks on `poll()` / `WSAPoll()` until the fd is ready for the given operation. |

---

## Server Class

**Header:** `Server.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Listens on a port and produces `Socket` objects for incoming connections.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `connection` | `std::unique_ptr<ConnectionServer>` | Polymorphic server connection. |
| `yield` | `YieldFunc` | Callback invoked when `accept()` would block. Default returns `false`. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `Server(ServerInit&& serverInit, Blocking blocking = Blocking::Yes)` | Creates `ConnectionType::SocketServer` or `ConnectionType::SSocketServer` via `std::visit`. |
| `Server(ServerInfo&&, Blocking)` | Convenience -- plain TCP. |
| `Server(SServerInfo&&, Blocking)` | Convenience -- TLS. |
| `Server(int port, Blocking)` | Convenience -- plain TCP on port. |
| `Server(int port, SSLctx&&, Blocking)` | Convenience -- TLS on port. |

### Move Semantics

| Signature | Description |
|-----------|-------------|
| `Server(Server&&) noexcept` | Move constructor. |
| `Server& operator=(Server&&) noexcept` | Move assignment. |
| `void swap(Server&) noexcept` | Swaps all members. |
| Copy construction/assignment | Deleted. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if connection is non-null and connected. |
| `socketId(Mode rw) const` | `int` | Underlying server socket fd. |
| `socketId() const` | `int` | Shorthand for `socketId(Mode::Read)`. |
| `close()` | `void` | Closes the server socket. |
| `release()` | `void` | Releases ownership of the server socket fd. |
| `accept(Blocking blocking, DeferAccept deferAccept)` | `Socket` | Accepts a connection. Calls `connection->accept(yield, blocking, deferAccept)` which returns a `unique_ptr<ConnectionClient>`. Wraps it in a `Socket` via the private `Socket(unique_ptr&&)` constructor. |
| `setYield(YieldFunc&&)` | `void` | Registers the accept yield callback. |

---

## BaseSocketStream\<Buffer\> Class

**Header:** `SocketStream.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

A `std::iostream` subclass that wraps a `Socket` via a `Buffer` (default `SocketStreamBuffer`).

```cpp
using SocketStream = BaseSocketStream<SocketStreamBuffer>;
```

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `buffer` | `Buffer` | The `SocketStreamBuffer` (or derived) instance. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `BaseSocketStream()` | Default -- disconnected. |
| `BaseSocketStream(Socket&& socket)` | Takes ownership of socket. |
| `BaseSocketStream(PipeInfo const&)` | Creates socket internally with `Blocking::No`. |
| `BaseSocketStream(FileInfo const&)` | Creates socket internally with `Blocking::No`. |
| `BaseSocketStream(SocketInfo const&)` | Creates socket internally with `Blocking::No`. |
| `BaseSocketStream(SSocketInfo const&)` | Creates socket internally with `Blocking::No`. |

### Move Semantics

Move-only. On move, the internal `rdbuf()` pointer is re-set to the new buffer.

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `close()` | `void` | Calls `getSocket().close()`. |
| `getSocket()` | `Socket&` / `Socket const&` | Access the underlying socket. |
| `operator bool() const` | `bool` | True if `buffer.getSocket().isConnected()`. |

---

## SocketStreamBuffer Class

**Header:** `SocketStreamBuffer.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

A `std::streambuf` subclass bridging standard stream I/O to a `Socket`. Uses internal 4KB input and output buffers (allocated as `std::vector<char>`).

**Output buffer trick:** The output area is set one byte shorter than the actual vector (`setp(&buf[0], &buf[0] + buf.size() - 1)`). This reserves one extra byte so that `overflow(ch)` can always safely write `*pptr() = ch` before flushing, avoiding data loss.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `socket` | `Socket` | The underlying socket. |
| `inputBuffer` | `std::vector<char>` | Input buffer (default 4KB). |
| `outputBuffer` | `std::vector<char>` | Output buffer (default 4KB). |
| `inCount` | `std::size_t` | Total bytes read from the socket (including buffered). |
| `outCount` | `std::size_t` | Total bytes written to the socket (including buffered). |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SocketStreamBuffer()` | Default -- disconnected. |
| `SocketStreamBuffer(Socket&& socket)` | Takes ownership of socket. |
| `SocketStreamBuffer(PipeInfo const&)` | Creates socket with `Blocking::No`. |
| `SocketStreamBuffer(FileInfo const&)` | Creates socket with `Blocking::No`. |
| `SocketStreamBuffer(SocketInfo const&)` | Creates socket with `Blocking::No`. |
| `SocketStreamBuffer(SSocketInfo const&)` | Creates socket with `Blocking::No`. |

### Move Semantics

Move-only via `SocketStreamBuffer(SocketStreamBuffer&&) noexcept` and move assignment.

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `getSocket()` | `Socket&` / `Socket const&` | Access the underlying socket. |
| `reserveInputSize(std::size_t size)` | `void` | Ensures input buffer is at least `size` bytes. Used by derived classes (e.g., MongoDB compressed streams). |
| `reserveOutputSize(std::size_t size)` | `void` | Ensures output buffer is at least `size` bytes. |

### Protected Methods (std::streambuf overrides)

| Method | Return | Description |
|--------|--------|-------------|
| `underflow()` | `int_type` | Called when input buffer is exhausted. First attempts non-blocking read (`tryGetMessageData`) to fill the entire buffer. If no data available, falls back to blocking read of at least 1 byte (`getMessageData`). Returns `traits::eof()` on connection close. |
| `xsgetn(char_type* s, std::streamsize count)` | `std::streamsize` | Optimized bulk read. For large reads (> half buffer size), reads directly into the destination buffer bypassing the internal buffer. |
| `overflow(int_type ch)` | `int_type` | Flushes the output buffer to the socket when full. Uses one extra byte of reserved space in the vector to avoid data loss when called with a character argument. |
| `xsputn(char_type const* s, std::streamsize count)` | `std::streamsize` | If data fits in remaining buffer space, copies it there. Otherwise flushes current buffer and writes directly to the socket. |
| `sync()` | `int` | Flushes the output buffer. Returns 0 on success, -1 on failure. |
| `seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)` | `std::streampos` | Only supports `std::ios_base::cur` with offset 0. Returns total bytes read or written (including buffered data). Useful for tracking stream position. |
| `swap(SocketStreamBuffer& rhs)` | `void` | Swaps all members including socket and buffers. |
| `incrementInCount(std::size_t size)` | `void` | Adds to `inCount`. |
| `incrementOutCount(std::size_t size)` | `void` | Adds to `outCount`. |
| `writeToStream(char const* data, std::size_t size)` | `std::streamsize` | Writes data to the socket via `socket.putMessageData()`. Used by derived classes. |
| `readFromStream(char* data, std::size_t size)` | `std::streamsize` | Reads data from the socket via `socket.getMessageData()`. Used by derived classes. |

### Destructor

The destructor calls `overflow()` to flush any remaining output data, catching and logging any exceptions (destructors must not throw).

---

## ConnectionBase Class

**Header:** `Connection.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Abstract base class for all connections. Non-copyable.

### Public Methods (all pure virtual unless noted)

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if the connection is open and valid. |
| `socketId(Mode) const` | `int` | Underlying file descriptor for the given mode (Read or Write). |
| `close()` | `void` | Close the connection. |
| `release()` | `void` | Release ownership of the fd without closing. |
| `externalyClosed()` | `void` | **Virtual, default no-op.** Notify the connection it was closed externally. |

---

## ConnectionClient Class

**Header:** `Connection.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Extends `ConnectionBase` for client-side (read/write) connections.

### Public Methods (all pure virtual unless noted)

| Method | Return | Description |
|--------|--------|-------------|
| `tryFlushBuffer()` | `void` | Flush/shutdown the write side of the connection. |
| `readFromStream(char* buffer, std::size_t size)` | `IOData` | Perform a **single** read operation (not looping). Returns bytes transferred and state. |
| `writeToStream(char const* buffer, std::size_t size)` | `IOData` | Perform a **single** write operation (not looping). Returns bytes transferred and state. |
| `protocol() const` | `std::string_view` | Returns protocol identifier (`"file"`, `"pipe"`, `"http"`, `"https"`). |
| `deferInit(YieldFunc&, YieldFunc&)` | `void` | **Virtual, default no-op.** Trigger deferred initialization (e.g., SSL handshake). |

---

## ConnectionServer Class

**Header:** `Connection.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Extends `ConnectionBase` for server-side (accept) connections.

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `accept(YieldFunc& yield, Blocking blocking, DeferAccept deferAccept)` | `std::unique_ptr<ConnectionClient>` | Accept an incoming connection. Blocks or yields as needed. Returns the new client connection. |

---

## FileDescriptor Class

**Header:** `ConnectionFileDescriptor.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Abstract intermediate class for connections using POSIX file descriptors.

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `tryFlushBuffer()` | `void` | **No-op** for file descriptor connections (only TCP sockets need flushing). |
| `readFromStream(char* buffer, std::size_t size)` | `IOData` | Calls POSIX `read()` on `getReadFD()`. Maps `errno` to `IOData` (see error handling below). |
| `writeToStream(char const* buffer, std::size_t size)` | `IOData` | Calls POSIX `write()` on `getWriteFD()`. Maps `errno` to `IOData`. |
| `getErrNoStr(int error)` | `char const*` | Returns a string name for an errno value. |

### Protected Methods (pure virtual, implemented by subclasses)

| Method | Return | Description |
|--------|--------|-------------|
| `getReadFD() const` | `int` | Returns the file descriptor for reading. |
| `getWriteFD() const` | `int` | Returns the file descriptor for writing. |

### Error Handling in readFromStream

| errno | IOData returned | Meaning |
|-------|-----------------|---------|
| (return 0) | `{0, false, false}` | EOF -- peer closed |
| `EINTR` | `{0, true, false}` | Interrupted -- retry immediately |
| `ECONNRESET` | `{0, false, false}` | Connection reset by peer |
| `EAGAIN`, `EWOULDBLOCK`, `ETIMEDOUT` | `{0, true, true}` | Would block |
| `EBADF`, `EFAULT`, `EINVAL`, `EISDIR`, `EBADMSG`, `ENXIO`, `ESPIPE` | throws `std::runtime_error` (Error) | Critical read error |
| `EOVERFLOW`, `ENOTCONN`, `EIO`, `ENOBUFS`, `ENOMEM`, (other) | throws `std::runtime_error` (Warning) | Unknown/recoverable error |

### Error Handling in writeToStream

| errno | IOData returned | Meaning |
|-------|-----------------|---------|
| `EINTR` | `{0, true, false}` | Interrupted -- retry immediately |
| `ENETUNREACH`, `ENETDOWN`, `ECONNRESET` | `{0, false, false}` | Connection lost |
| `EAGAIN`, `EWOULDBLOCK` | `{0, true, true}` | Would block |
| `EBADF`, `EFAULT`, `EINVAL`, `ENXIO`, `ESPIPE`, `EDESTADDRREQ`, `EPIPE` | throws `std::runtime_error` (Error) | Critical write error |
| `EACCES`, `ERANGE`, `ENOTCONN`, `EIO`, `ENOBUFS`, `EDQUOT`, `EFBIG`, `ENOSPC`, `EPERM`, (other) | throws `std::runtime_error` (Warning) | Unknown/recoverable error |

---

## SimpleFile Class

**Header:** `ConnectionSimpleFile.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Wraps a single file descriptor opened via `open()`.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `fd` | `int` | The file descriptor. -1 when closed/released. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SimpleFile(FileInfo const& fileInfo, Blocking blocking)` | Opens the file with `open()`. Sets non-blocking if requested. |
| `SimpleFile(int fd)` | Wraps an existing fd. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if `fd != -1`. |
| `socketId(Mode) const` | `int` | Returns `fd` (same for both Read and Write). |
| `close()` | `void` | Calls `::close(fd)` and sets `fd = -1`. |
| `release()` | `void` | Sets `fd = -1` without closing. |
| `getReadFD() const` | `int` | Returns `fd`. |
| `getWriteFD() const` | `int` | Returns `fd`. |
| `protocol() const` | `std::string_view` | Returns `"file"`. |

---

## Pipe Class

**Header:** `ConnectionPipe.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Wraps a pipe (two file descriptors).

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `fd[2]` | `int[2]` | `fd[0]` = read end, `fd[1]` = write end. -1 when closed. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `Pipe(PipeInfo const&, Blocking blocking)` | Creates a pipe via `pipe()` / `_pipe()`. Sets non-blocking if requested. |
| `Pipe(int fd[])` | Wraps existing pipe fds. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if both fds are not -1. |
| `socketId(Mode::Read) const` | `int` | Returns `fd[0]`. |
| `socketId(Mode::Write) const` | `int` | Returns `fd[1]`. |
| `close()` | `void` | Closes both fds. |
| `release()` | `void` | **Throws `std::runtime_error`** -- pipes cannot be released. |
| `getReadFD() const` | `int` | Returns `fd[0]`. |
| `getWriteFD() const` | `int` | Returns `fd[1]`. |
| `protocol() const` | `std::string_view` | Returns `"pipe"`. |

---

## SocketStandard Class

**Header:** `ConnectionSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Internal helper managing a TCP socket file descriptor. **Not** part of the `ConnectionBase` hierarchy. Used via composition by `SocketClient` and `SocketServer`.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `fd` | `SOCKET_TYPE` | The socket fd. `thorInvalidFD()` when closed. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SocketStandard(ServerInfo const&, Blocking)` | Creates socket, sets `SO_REUSEADDR`, binds to `INADDR_ANY` on port, listens with backlog of 5. |
| `SocketStandard(SocketInfo const&, Blocking)` | Creates socket, resolves host via `gethostbyname()`, connects. |
| `SocketStandard(OpenSocketInfo const&, Blocking)` | Wraps an existing fd. Sets blocking mode. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if `fd != thorInvalidFD()`. |
| `socketId(Mode) const` | `int` | Returns `fd` (same for both modes). |
| `close()` | `void` | Calls `thorCloseSocket(fd)`. |
| `release()` | `void` | Sets `fd = thorInvalidFD()` without closing. |
| `getFD() const` | `int` | Returns `fd`. |

### Private Methods

| Method | Description |
|--------|-------------|
| `createSocket()` | Calls `::socket(AF_INET, SOCK_STREAM, 0)`. |
| `setUpBlocking(Blocking)` | Calls `thorSetSocketNonBlocking()` if `Blocking::No`. |
| `setUpServerSocket(ServerInfo const&)` | Sets `SO_REUSEADDR`, binds, listens. |
| `setUpClientSocket(SocketInfo const&)` | Resolves hostname, connects. |

---

## SocketClient Class

**Header:** `ConnectionSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Concrete connection for plain TCP clients. On Unix, inherits from `FileDescriptor`. On Windows, inherits from `ConnectionClient` directly.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `socketInfo` | `SocketStandard` | Manages the TCP socket fd. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SocketClient(SocketInfo const&, Blocking)` | Normal client connection. |
| `SocketClient(SocketServer&, OpenSocketInfo const&, Blocking)` | Server-side accepted connection (wraps an already-accepted fd). |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | Delegates to `socketInfo.isConnected()`. |
| `socketId(Mode) const` | `int` | Delegates to `socketInfo.socketId()`. |
| `close()` | `void` | Delegates to `socketInfo.close()`. |
| `release()` | `void` | Delegates to `socketInfo.release()`. |
| `tryFlushBuffer()` | `void` | Calls `thorShutdownSocket()` on the fd, signaling write-end done. |
| `protocol() const` | `std::string_view` | Returns `"http"`. |
| `getReadFD() const` | `int` | (Unix) Returns `socketInfo.getFD()`. |
| `getWriteFD() const` | `int` | (Unix) Returns `socketInfo.getFD()`. |
| `readFromStream(...)` | `IOData` | (Windows) Uses `recv()`. (Unix) Inherited from `FileDescriptor`. |
| `writeToStream(...)` | `IOData` | (Windows) Uses `send()`. (Unix) Inherited from `FileDescriptor`. |

---

## SocketServer Class

**Header:** `ConnectionSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Concrete connection for plain TCP servers.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `socketInfo` | `SocketStandard` | Manages the listening socket fd. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | Delegates to `socketInfo.isConnected()`. |
| `socketId(Mode) const` | `int` | Delegates to `socketInfo.socketId()`. |
| `close()` | `void` | Delegates to `socketInfo.close()`. |
| `release()` | `void` | Delegates to `socketInfo.release()`. |
| `accept(YieldFunc& yield, Blocking, DeferAccept)` | `unique_ptr<ConnectionClient>` | Calls `acceptSocket(yield)` to get a new fd, wraps it in a `SocketClient`. |

### Protected Methods

| Method | Return | Description |
|--------|--------|-------------|
| `acceptSocket(YieldFunc& yield)` | `int` | Calls `::accept()`. On `EWOULDBLOCK`/`EAGAIN`, invokes yield (if returns true, retry; if false, calls `waitForFileDescriptor()` which uses `poll()`). |
| `waitForFileDescriptor(int fd)` | `void` | Blocks on `poll()` until the fd is readable. |
| `wouldBlock(int errorCode)` | `bool` | Returns true for `EAGAIN` and `EWOULDBLOCK` (Unix) or `WSAEWOULDBLOCK` (Windows). Any other error code in `acceptSocket()` throws. |

---

## SSocketStandard Class

**Header:** `ConnectionSSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Internal helper managing the OpenSSL `SSL*` object for a secure connection.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `ssl` | `SSL*` | The OpenSSL SSL object. |
| `connectionFailed` | `bool` | Set to true if a fatal SSL error occurred. |
| `deferAction` | `DeferAction` | `None`, `Connect`, or `Accept` -- tracks deferred handshake. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SSocketStandard(SSocketInfo const&, int fd)` | Creates SSL object, associates with fd. Performs or defers client handshake based on `SSocketInfo::defer`. |
| `SSocketStandard(OpenSSocketInfo const&, int fd)` | Same for server-accepted connections. |
| Copy/move constructors | All deleted. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | True if `ssl != nullptr`. Note: does **not** check `connectionFailed`. |
| `close()` | `void` | If `ssl != nullptr`: if `!connectionFailed`, calls `SSL_shutdown()` in a loop (retrying on return value 0 or `SSL_ERROR_WANT_READ`; breaks and logs on other errors). Then calls `SSL_free()` and sets `ssl = nullptr`. If `connectionFailed`, skips `SSL_shutdown()` entirely (only frees). |
| `externalyClosed()` | `void` | Sets `connectionFailed = true` to skip `SSL_shutdown()` during close. |
| `buildSSErrorMessage(int)` | `std::string` | Builds error message from OpenSSL error queue. |
| `getSSL() const` | `SSL*` | Returns the SSL object. |
| `checkConnectionOK(int errorCode)` | `void` | If `errorCode` is `SSL_ERROR_SYSCALL` or `SSL_ERROR_SSL`, sets `connectionFailed = true`. |
| `deferInit(YieldFunc& rYield, YieldFunc& wYield)` | `void` | Executes the deferred handshake (`initSSocketClientConnect` or `initSSocketClientAccept`). |

### Private Methods

| Method | Description |
|--------|-------------|
| `initSSocket(SSLctx const& ctx, int fd)` | Calls `SSL_new(ctx.ctx)`, then `SSL_set_fd(ssl, fd)`. On failure, frees SSL object and throws. |
| `initSSocketClientConnect(YieldFunc&, YieldFunc&)` | Calls `SSL_connect()` in a loop, retrying on `SSL_ERROR_WANT_READ` (yielding via `rYield`), `SSL_ERROR_WANT_WRITE`/`SSL_ERROR_WANT_CONNECT` (yielding via `wYield`). On success, verifies peer certificate via `SSL_get1_peer_certificate()` (throws if no cert). |
| `initSSocketClientAccept(YieldFunc&, YieldFunc&)` | Calls `SSL_accept()` in a loop, retrying on `SSL_ERROR_WANT_READ` (yields `rYield`), `SSL_ERROR_WANT_WRITE`/`SSL_ERROR_WANT_ACCEPT` (yields `wYield`). On success, checks `SSL_get_verify_result()` equals `X509_V_OK` (logs and nulls SSL on failure). |

---

## SSocketClient Class

**Header:** `ConnectionSSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Extends `SocketClient` with SSL encryption.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `secureSocketInfo` | `SSocketStandard` | Manages the SSL object. |

### Constructors

| Signature | Description |
|-----------|-------------|
| `SSocketClient(SSocketInfo const&, Blocking)` | Normal TLS client. |
| `SSocketClient(SSocketServer&, OpenSSocketInfo const&, Blocking)` | Server-side accepted TLS connection. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `isConnected() const` | `bool` | Delegates to `secureSocketInfo.isConnected()` only (checks `ssl != nullptr`). Does **not** check the base `SocketClient`. |
| `close()` | `void` | Closes SSL then the underlying socket. |
| `externalyClosed()` | `void` | Notifies `secureSocketInfo` then calls `SocketClient::externalyClosed()`. |
| `protocol() const` | `std::string_view` | Returns `"https"`. |
| `readFromStream(char*, std::size_t)` | `IOData` | Uses `SSL_read()`. Maps SSL errors to `IOData` (see below). |
| `writeToStream(char const*, std::size_t)` | `IOData` | Uses `SSL_write()`. Maps SSL errors to `IOData`. |
| `deferInit(YieldFunc&, YieldFunc&)` | `void` | Delegates to `secureSocketInfo.deferInit()`. |

### SSL Error Mapping (readFromStream)

| SSL Error | IOData | Meaning |
|-----------|--------|---------|
| `SSL_ERROR_NONE` | `{0, true, false}` | No error, retry |
| `SSL_ERROR_ZERO_RETURN` | `{0, false, false}` | Peer sent `close_notify` |
| `SSL_ERROR_WANT_READ` | `{0, true, true}` | Blocked -- need more data |
| `SSL_ERROR_WANT_WRITE`, `SSL_ERROR_WANT_CONNECT`, `SSL_ERROR_WANT_ACCEPT`, `SSL_ERROR_SYSCALL`, `SSL_ERROR_SSL` | throws `std::runtime_error` (Error) | Critical SSL error |
| `SSL_ERROR_WANT_X509_LOOKUP`, `SSL_ERROR_WANT_CLIENT_HELLO_CB`, `SSL_ERROR_WANT_ASYNC`, `SSL_ERROR_WANT_ASYNC_JOB`, (other) | throws `std::runtime_error` (Warning) | Unknown SSL error |

### SSL Error Mapping (writeToStream)

| SSL Error | IOData | Meaning |
|-----------|--------|---------|
| `SSL_ERROR_NONE` | `{0, true, false}` | No error, retry |
| `SSL_ERROR_ZERO_RETURN` | `{0, false, false}` | Peer sent `close_notify` |
| `SSL_ERROR_WANT_WRITE` | `{0, true, true}` | Blocked -- buffer full |
| `SSL_ERROR_WANT_READ`, `SSL_ERROR_WANT_CONNECT`, `SSL_ERROR_WANT_ACCEPT`, `SSL_ERROR_SYSCALL`, `SSL_ERROR_SSL` | throws `std::runtime_error` (Error) | Critical SSL error |
| `SSL_ERROR_WANT_X509_LOOKUP`, `SSL_ERROR_WANT_CLIENT_HELLO_CB`, `SSL_ERROR_WANT_ASYNC`, `SSL_ERROR_WANT_ASYNC_JOB`, (other) | throws `std::runtime_error` (Warning) | Unknown SSL error |

Note: Both `readFromStream` and `writeToStream` call `secureSocketInfo.checkConnectionOK(errorCode)` which sets `connectionFailed = true` for `SSL_ERROR_SYSCALL` or `SSL_ERROR_SSL`. This causes subsequent `isConnected()` checks to return `false` and prevents `SSL_shutdown()` from being called during cleanup.

---

## SSocketServer Class

**Header:** `ConnectionSSocket.h` | **Namespace:** `ThorsAnvil::ThorsSocket::ConnectionType`

Extends `SocketServer` for TLS.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `ctx` | `SSLctx` | Owns the SSL context for this server. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `accept(YieldFunc&, Blocking, DeferAccept)` | `unique_ptr<ConnectionClient>` | Calls inherited `acceptSocket()` to get a new fd, wraps it in an `SSocketClient` with the server's `ctx`. |

---

## SSLUtil Class

**Header:** `SecureSocketUtil.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Singleton that initializes the OpenSSL library.

### Private Members / Constructor

| Member | Description |
|--------|-------------|
| `SSLUtil()` | Private. Calls `SSL_load_error_strings()`, `SSL_library_init()`. |

### Public Methods

| Method | Return | Description |
|--------|--------|-------------|
| `getInstance()` | `SSLUtil&` | Static. Returns the singleton instance (constructed on first call). |

---

## SSLctx Class

**Header:** `SecureSocketUtil.h` | **Namespace:** `ThorsAnvil::ThorsSocket`

Wraps an `SSL_CTX*` object.

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `ctx` | `SSL_CTX*` | The OpenSSL context. `nullptr` when moved-from. |

### Constructor

```cpp
template<typename... Args>
SSLctx(SSLMethodType methodType, Args&&... args);
```

1. Calls `SSLUtil::getInstance()` to ensure OpenSSL is initialized.
2. Creates the SSL method (`TLS_client_method()` or `TLS_server_method()`).
3. Creates the context via `SSL_CTX_new()`.
4. Applies each configuration argument via fold expression: `(args.apply(ctx), ...)`.

### Move Semantics

Move-only. On move, source `ctx` is set to `nullptr` via `std::exchange`.

### Private Methods

| Method | Return | Description |
|--------|--------|-------------|
| `createClient()` | `SSL_METHOD const*` | Returns `TLS_client_method()`. |
| `createServer()` | `SSL_METHOD const*` | Returns `TLS_server_method()`. |
| `newCtx(SSL_METHOD const*)` | `SSL_CTX*` | Returns `SSL_CTX_new(method)`. |

---

## ProtocolInfo Struct

**Header:** `SecureSocketUtil.h`

### Private Members

| Member | Type | Default |
|--------|------|---------|
| `minProtocol` | `Protocol` | `TLS_1_2` |
| `maxProtocol` | `Protocol` | `TLS_1_3` |

### Public Methods

| Method | Description |
|--------|-------------|
| `apply(SSL_CTX*)` | Sets min/max protocol version on the context via `SSL_CTX_set_min_proto_version()` / `SSL_CTX_set_max_proto_version()`. |
| `apply(SSL*)` | Sets min/max protocol version on a specific SSL object. |

### Private Methods

| Method | Description |
|--------|-------------|
| `convertProtocolToOpenSSL(Protocol)` | Maps the `Protocol` enum to OpenSSL constants (`TLS1_VERSION`, `TLS1_1_VERSION`, etc.). |

---

## CipherInfo Struct

**Header:** `SecureSocketUtil.h`

### Public Members

| Member | Type | Default |
|--------|------|---------|
| `cipherList` | `std::string` | ECDHE-based AES-GCM and CHACHA20-POLY1305 ciphers (for TLS 1.2 and below) |
| `cipherSuite` | `std::string` | TLS 1.3 cipher suites (AES-256-GCM, CHACHA20-POLY1305, AES-128-GCM) |

### Public Methods

| Method | Description |
|--------|-------------|
| `apply(SSL_CTX*)` | Calls `SSL_CTX_set_cipher_list()` and `SSL_CTX_set_ciphersuites()`. |
| `apply(SSL*)` | Sets ciphers on a specific SSL object. |

---

## CertificateInfo Struct

**Header:** `SecureSocketUtil.h`

### Private Members

| Member | Type | Description |
|--------|------|-------------|
| `certificateFileName` | `std::string` | Path to the PEM certificate file. |
| `keyFileName` | `std::string` | Path to the PEM private key file. |
| `hasPasswordGetter` | `bool` | Whether a password callback was provided. |
| `getPassword` | `GetPasswordFunc` | `std::function<std::string(int)>` returning the password. |

### Public Methods

| Method | Description |
|--------|-------------|
| `apply(SSL_CTX*)` | Loads certificate and key files. If `hasPasswordGetter`, sets the OpenSSL password callback. Password is securely overwritten after use. |
| `apply(SSL*)` | Loads certificate and key on a specific SSL object. |

---

## CertifcateAuthorityInfo Struct

**Header:** `SecureSocketUtil.h`

### Public Members

| Member | Type | Description |
|--------|------|-------------|
| `file` | `CertifcateAuthorityDataInfo<File>` | CA files. |
| `dir` | `CertifcateAuthorityDataInfo<Dir>` | CA directories. |
| `store` | `CertifcateAuthorityDataInfo<Store>` | CA stores. |

Each `CertifcateAuthorityDataInfo<A>` has:

| Member | Type | Description |
|--------|------|-------------|
| `loadDefault` | `bool` | Load system default CA trust store. Default `false`. |
| `items` | `std::vector<std::string>` | Additional CA paths to load. |

### Public Methods

| Method | Description |
|--------|-------------|
| `apply(SSL_CTX*)` | Applies file, dir, and store CA settings to the context. |

---

## ClientCAListInfo Struct

**Header:** `SecureSocketUtil.h`

### Public Members

| Member | Type | Description |
|--------|------|-------------|
| `verifyClientCA` | `bool` | If true, configures `SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT`. Default `false`. |
| `file` | `ClientCAListDataInfo<File>` | Client CA files. |
| `dir` | `ClientCAListDataInfo<Dir>` | Client CA directories. |
| `store` | `ClientCAListDataInfo<Store>` | Client CA stores. |

### Public Methods

| Method | Description |
|--------|-------------|
| `buildCAToList()` | Builds a `STACK_OF(X509_NAME)*` from all configured sources. |
| `apply(SSL_CTX*)` | Sets verification mode and CA list on the context. |
| `apply(SSL*)` | Sets verification mode and CA list on a specific SSL object. |

---

## Platform Abstraction (ConnectionUtil)

**Header:** `ConnectionUtil.h`

Platform-specific functions abstracted behind `thor*()` helpers and `THOR_*` macros:

| Function / Macro | Unix | Windows |
|------------------|------|---------|
| `SOCKET_TYPE` | `int` | `SOCKET` |
| `thorCreatePipe(int fd[2])` | `pipe()` | `_pipe()` |
| `thorSetFDNonBlocking(int fd)` | `fcntl(fd, F_SETFL, O_NONBLOCK)` | -- |
| `thorSetSocketNonBlocking(fd)` | `fcntl(fd, F_SETFL, O_NONBLOCK)` | `ioctlsocket(fd, FIONBIO, ...)` |
| `thorCloseSocket(fd)` | `::close(fd)` | `closesocket(fd)` |
| `thorShutdownSocket(fd)` | `shutdown(fd, SHUT_WR)` | `shutdown(fd, SD_SEND)` |
| `thorGetSocketError()` | `errno` | `WSAGetLastError()` |
| `thorInvalidFD()` | `-1` | `INVALID_SOCKET` |
| `thorErrorIsTryAgain(error)` | `error == TRY_AGAIN` | `error == WSATRY_AGAIN` |
| `THOR_POLL_TYPE` | `pollfd` | `WSAPOLLFD` |
| `THOR_POLL` | `poll` | `WSAPoll` |
| `THOR_SOCKET_ID(x)` | `x` | `static_cast<SOCKET>(x)` |

Also provides OpenSSL X509 stack wrappers: `sk_X509_NAME_new_null_wrapper()`, `sk_X509_NAME_free_wrapper()`, `sk_X509_NAME_pop_free_wrapper()`.

---

## Design Patterns

### Strategy Pattern (Connection Hierarchy)

`Socket` and `Server` use `unique_ptr<ConnectionClient>` / `unique_ptr<ConnectionServer>` to hold a polymorphic connection. Concrete type selected at construction via `std::visit`.

### Variant-Based Construction

`SocketInit` and `ServerInit` are `std::variant` types. Constructors use `std::visit` with a builder functor defined in the `.cpp` files:

```cpp
// Socket.cpp
struct SocketConnectionBuilder {
    Blocking blocking;
    unique_ptr<ConnectionClient> operator()(FileInfo const&)   { return make_unique<SimpleFile>(...); }
    unique_ptr<ConnectionClient> operator()(PipeInfo const&)   { return make_unique<Pipe>(...); }
    unique_ptr<ConnectionClient> operator()(SocketInfo const&) { return make_unique<SocketClient>(...); }
    unique_ptr<ConnectionClient> operator()(SSocketInfo const&){ return make_unique<SSocketClient>(...); }
};

// Server.cpp
struct ServerConnectionBuilder {
    Blocking blocking;
    unique_ptr<ConnectionServer> operator()(ServerInfo&&)  { return make_unique<SocketServer>(...); }
    unique_ptr<ConnectionServer> operator()(SServerInfo&&) { return make_unique<SSocketServer>(...); }
};
```

This pattern keeps the concrete connection types hidden in the implementation files.

### Composition over Inheritance (SocketStandard, SSocketStandard)

Socket management logic is in standalone helper classes used as composition members, avoiding diamond inheritance.

### Template Method Pattern (FileDescriptor)

`FileDescriptor` implements `readFromStream()` / `writeToStream()` using virtual `getReadFD()` / `getWriteFD()`.

### Deferred Initialization

SSL handshake can be deferred via `DeferAccept::Yes`. `SSocketStandard` stores a `DeferAction` and executes the handshake when `deferInit()` is called.

---

## Header-Only Mode

When `THORS_SOCKET_HEADER_ONLY` is defined, each header includes a corresponding `.source` file:

```cpp
#if THORS_SOCKET_HEADER_ONLY
#include "Socket.source"
#endif
```

The `THORS_SOCKET_HEADER_ONLY_INCLUDE` macro controls whether functions have external linkage or are inlined.

---

## Testing and Mocking

The codebase uses `MOCK_FUNC()` / `MOCK_TFUNC()` macros wrapping system calls and OpenSSL functions. In production builds these resolve to real functions. In test builds they resolve to mockable function pointers.

The `TestMarker` enum and template constructor on `Socket` allow unit tests to inject mock connection implementations:

```cpp
Socket socket(TestMarker::True, mockConnectionInfo);
```

Test files are in `src/ThorsSocket/test/`. `MockHeaderInclude.h` provides the mock infrastructure. `ConnectionTest.h` provides common test utilities.
