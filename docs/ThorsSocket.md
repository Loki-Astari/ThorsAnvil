[Home](index.html) | [Internal Documentation](internal/ThorsSocket.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsSocket API

A C++ socket library that provides a unified, type-safe abstraction over files, pipes, TCP sockets, and SSL/TLS sockets. All transport types share the same `Socket` and `SocketStream` interface, so your application code works identically regardless of the underlying transport.

**Namespace:** `ThorsAnvil::ThorsSocket`

---

## Quick Start

### Makefile
```Makefile
CXXFLAGS    = -std=c++20

LDLIBS      = -lThorsLogging -lThorSerialize -lThorsSocket

all:        ThorsSocketApp
```
### ThorsSocketApp.cpp

```cpp
#include "ThorsSocket/SocketStream.h"
#include <iostream>

using namespace ThorsAnvil::ThorsSocket;

int main()
{
    // Connect to a server
    SocketStream stream{SocketInfo{"example.com", 80}};

    // Use standard C++ stream operations
    stream << "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n" << std::flush;

    std::string line;
    while (std::getline(stream, line)) {
        std::cout << line << "\n";
    }
}
```

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorsSocket/SocketStream.h` | `SocketStream` -- `std::iostream` wrapper over a `Socket` |
| `ThorsSocket/Socket.h` | `Socket` class (client-side I/O) |
| `ThorsSocket/Server.h` | `Server` class (listening for connections) |

---

## SocketStream

A `std::iostream` that wraps a `Socket`, allowing standard C++ stream operations over any connection type.

### Creating a SocketStream

```cpp
// From a file
SocketStream stream{FileInfo{"data.txt", FileMode::Read}};

// From a pipe
SocketStream stream{PipeInfo{}};

// TCP connection
SocketStream stream{SocketInfo{"hostname", 8080}};
SocketStream stream{SocketInfo{"hostname", 8080}, Blocking::No};


// TLS connection
SSLctx ctx{SSLMethodType::Client};
SocketStream stream{SSocketInfo{"hostname", 443, ctx}};
SocketStream stream{SSocketInfo{"hostname", 443, ctx}, Blocking::No};

```

### Using with Standard C++ I/O

```cpp
SocketStream stream{SocketInfo{"api.example.com", 80}};

// Write using operator<<
stream << "GET /data HTTP/1.1\r\n"
       << "Host: api.example.com\r\n\r\n"
       << std::flush;

// Read using standard stream operations
std::string line;
std::getline(stream, line);

// Check connection state
if (stream) {
    // stream is still connected
}
```

### Accessing the Underlying Socket

```cpp
Socket& sock = stream.getSocket();
```

---

## Socket

The primary class for reading from and writing to a connection.

### Creating a Socket

{% raw %}
```cpp
// From a file
Socket socket{FileInfo{"data.txt", FileMode::Read}};

// From a pipe
Socket socket{PipeInfo{}};

// TCP connection
Socket socket{SocketInfo{"hostname", 8080}};
Socket socket{SocketInfo{"hostname", 8080}, Blocking::No};


// TLS connection
SSLctx ctx{SSLMethodType::Client};
Socket socket{SSocketInfo{"hostname", 443, ctx}};
Socket socket{SSocketInfo{"hostname", 443, ctx}, Blocking::No};

```
{% endraw %}

### Reading and Writing

```cpp
// Read exactly 'size' bytes (blocks until complete or connection closes)
char buffer[1024];
IOData result = socket.getMessageData(buffer, sizeof(buffer));

// Write all data (blocks until complete)
IOData result = socket.putMessageData(data, dataSize);

// Non-blocking variants (return immediately if would block)
IOData result = socket.tryGetMessageData(buffer, sizeof(buffer));
IOData result = socket.tryPutMessageData(data, dataSize);
```

### IOData Result

Every read/write operation returns an `IOData` struct:

```cpp
struct IOData {
    std::size_t dataSize;    // bytes transferred
    bool        stillOpen;   // false if connection closed
    bool        blocked;     // true if operation would block
};
```

### Connection State

```cpp
if (socket.isConnected()) {
    int fd = socket.socketId();
    socket.close();
}
```

### Move Semantics

`Socket` is move-only. The moved-from socket is left disconnected.

```cpp
Socket a{SocketInfo{"host", 80}};
Socket b = std::move(a);  // 'a' is now disconnected
```

---

## Server

Listens on a port and produces `Socket` objects for incoming connections.

### Creating a Server

```cpp
// Plain TCP server
Server server{ServerInfo{8080}};

// TLS server
CertificateInfo cert{"fullchain.pem", "privkey.pem"};
SSLctx ctx{SSLMethodType::Server, cert};
Server server{SServerInfo{8443, std::move(ctx)}};
```

### Accepting Connections

```cpp
// Block until a client connects
Socket client = server.accept();
```

---

## SSL/TLS Configuration

### SSLctx

Wraps an OpenSSL `SSL_CTX`. Accepts any combination of configuration objects:

```cpp
// Minimal client context (TLS 1.2+)
SSLctx clientCtx{SSLMethodType::Client};

// Server context with certificate
CertificateInfo cert{"fullchain.pem", "privkey.pem"};
SSLctx serverCtx{SSLMethodType::Server, cert};

// Full configuration
SSLctx ctx{SSLMethodType::Client,
           ProtocolInfo{Protocol::TLS_1_2, Protocol::TLS_1_3},
           CipherInfo{},  // secure defaults
           cert};
```

### ProtocolInfo

Constrains the TLS protocol version range:

```cpp
ProtocolInfo proto{Protocol::TLS_1_2, Protocol::TLS_1_3};
```

Available protocols: `TLS_1_0`, `TLS_1_1`, `TLS_1_2`, `TLS_1_3`.

### CertificateInfo

Specifies certificate and private key files (PEM format):

```cpp
// Basic
CertificateInfo cert{"fullchain.pem", "privkey.pem"};

// With password callback
CertificateInfo cert{"fullchain.pem", "privkey.pem",
    [](int /*rwflag*/) { return std::string{"mypassword"}; }};
```

### CertifcateAuthorityInfo

Configures CA trust stores for verifying peer certificates:

```cpp
CertifcateAuthorityInfo ca;
ca.file.loadDefault = true;                     // load system defaults
ca.file.items.push_back("/path/to/ca-cert.pem"); // additional CA file
```

### ClientCAListInfo

Configures client certificate verification on servers:

```cpp
ClientCAListInfo clientCA;
clientCA.verifyClientCA = true;
clientCA.file.items.push_back("/path/to/client-ca.pem");

SSLctx ctx{SSLMethodType::Server, cert, clientCA};
```

---

## Initialization Structs

| Struct | Purpose | Key Fields |
|--------|---------|------------|
| `FileInfo` | Open a file | `fileName`, `mode` (Read, WriteAppend, WriteTruncate) |
| `PipeInfo` | Create a pipe | (no fields) |
| `SocketInfo` | TCP connection | `host`, `port` |
| `ServerInfo` | TCP server | `port` |
| `SSocketInfo` | TLS connection | `host`, `port`, `ctx`, `defer` |
| `SServerInfo` | TLS server | `port`, `ctx` |

---

## Enumerations

```cpp
enum class FileMode    { Read, WriteAppend, WriteTruncate };
enum class Blocking    { No, Yes };
enum class Mode        { Read, Write };
enum class DeferAccept { No, Yes };
```

---

## Asynchronous / Coroutine Support

ThorsSocket supports non-blocking I/O through yield callbacks. When a blocking operation would occur, the registered yield function is called, allowing integration with coroutine libraries or event loops:

This is the mechanism used by NisseServer to provide transparent async I/O with synchronous-looking code.

```cpp
Socket socket{SocketInfo{"host", 80}, Blocking::No};

// Register yield callbacks
socket.setReadYield([]() -> bool {
    // Called when read would block
    // Return true to retry immediately, false to fall back to poll()
    myCoroutineYield();
    return true;
});

socket.setWriteYield([]() -> bool {
    myCoroutineYield();
    return true;
});
```

---

## Complete Example: TLS Client

{% raw %}
```cpp
#include "ThorsSocket/Socket.h"
#include "ThorsSocket/SocketStream.h"
#include "ThorsSocket/SecureSocketUtil.h"
#include <iostream>
#include <string>

using namespace ThorsAnvil::ThorsSocket;

int main()
{
    // Create TLS context
    SSLctx ctx{SSLMethodType::Client};

    // Connect
    SocketStream stream{SSocketInfo{"example.com", 443, ctx}};

    // Send HTTP request
    stream << "GET / HTTP/1.1\r\n"
           << "Host: example.com\r\n"
           << "Connection: close\r\n\r\n"
           << std::flush;

    // Read response
    std::string line;
    while (std::getline(stream, line)) {
        std::cout << line << "\n";
    }
}
```
{% endraw %}

## Complete Example: TLS Server

```cpp
#include "ThorsSocket/Server.h"
#include "ThorsSocket/SocketStream.h"
#include <iostream>

using namespace ThorsAnvil::ThorsSocket;

int main()
{
    // Information about SSL Certificates
    // https://lokiastari.com/posts/NisseV3
    //
    // Getting a free SSL Certificate for your site:
    // https://letsencrypt.org/
    //
    CertificateInfo cert{"fullchain.pem", "privkey.pem"};
    SSLctx ctx{SSLMethodType::Server, cert};
    Server server{SServerInfo{8080, ctx}};
    std::cout << "Listening on port 8080\n";

    while (true) {
        Socket client = server.accept();
        SocketStream stream{std::move(client)};

        std::string request;
        std::getline(stream, request);
        std::cout << "Received: " << request << "\n";

        std::string body = "Hello from ThorsSocket!";
        stream << "HTTP/1.1 200 OK\r\n"
               << "Content-Length: " << body.size() << "\r\n\r\n"
               << body << std::flush;
    }
}
```
