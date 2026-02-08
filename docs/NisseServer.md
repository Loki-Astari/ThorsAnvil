# NisseServer API

An event-driven, coroutine-based C++ server framework. Write synchronous-looking request handlers while the framework handles non-blocking async I/O, thread pools, and connection management behind the scenes.

**Namespace:** `ThorsAnvil::Nisse::Server`

---

## Quick Start

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseServer/Pynt.h"
#include <ThorsSocket/SocketStream.h>

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace TASock    = ThorsAnvil::ThorsSocket;

// Define a protocol handler
class EchoProtocol : public NisServer::Pynt {
public:
    NisServer::PyntResult handleRequest(
        TASock::SocketStream& stream,
        NisServer::Context& context) override
    {
        std::string line;
        std::getline(stream, line);           // reads asynchronously
        stream << "Echo: " << line << "\n";   // writes asynchronously
        return NisServer::PyntResult::More;   // keep connection open
    }
};

int main()
{
    NisServer::NisseServer server;
    EchoProtocol echo;

    server.listen(TASock::ServerInfo{8080}, echo);
    server.run();  // blocks until stopped
}
```

Handler code reads as synchronous -- no callbacks, no futures, no explicit state machines. The framework uses `boost::coroutines2` to yield transparently when I/O would block.

---

## Headers

| Header | Purpose |
|--------|---------|
| `NisseServer/NisseServer.h` | Main `NisseServer` class |
| `NisseServer/Pynt.h` | Abstract protocol handler interface |
| `NisseServer/PyntControl.h` | Simple TCP control port (stops server on connect) |
| `NisseServer/Context.h` | Per-connection context and async socket registration |
| `NisseServer/TimerAction.h` | Abstract timer callback interface |

---

## NisseServer

The central server object. Owns the event loop, thread pool, and connection state.

### Construction

```cpp
NisseServer server;                // 1 worker thread (default)
NisseServer server(4);             // 4 worker threads
```

### Listening

```cpp
// Plain TCP
server.listen(TASock::ServerInfo{8080}, myHandler);

// TLS
TASock::CertificateInfo cert{"fullchain.pem", "privkey.pem"};
TASock::SSLctx ctx{TASock::SSLMethodType::Server, cert};
server.listen(TASock::SServerInfo{8443, std::move(ctx)}, myHandler);
```

Call `listen()` multiple times to listen on multiple ports with different handlers.

### Running and Stopping

```cpp
server.run();        // blocks until stopped

server.stopSoft();   // graceful: finish active connections, then stop
server.stopHard();   // immediate: stop on next timer tick
```

### Timers

```cpp
class MyTimer : public NisServer::TimerAction {
    void handleRequest(int timerId) override {
        std::cout << "Timer fired!\n";
    }
};

MyTimer timer;
server.addTimer(std::chrono::seconds(5), timer);  // fires every 5 seconds
```

---

## Pynt (Protocol Handler)

The abstract base class for protocol handlers. Implement this for any non-HTTP protocol.

```cpp
class Pynt {
public:
    virtual PyntResult handleRequest(TASock::SocketStream& stream, Context& context) = 0;
};
```

**Return values:**

| Value | Meaning |
|-------|---------|
| `PyntResult::Done` | Connection finished; server closes the socket |
| `PyntResult::More` | Request done but keep connection open (e.g., keep-alive) |

The `stream` parameter is a `std::iostream` wrapping the client socket. Reads and writes are transparently non-blocking -- the coroutine yields when I/O would block.

---

## Context

Passed into every `handleRequest()` call. Provides access to the server's async I/O system for opening additional connections.

### Registering Additional Sockets

Within a handler, you can open connections to databases, upstream servers, etc., and make them participate in the async event loop:

```cpp
// Open a connection to an upstream server
TASock::SocketStream upstream{TASock::Socket{
    TASock::SocketInfo{"backend.local", 9090}, TASock::Blocking::No}};

// Register with event loop (RAII -- unregisters on scope exit)
NisServer::AsyncStream async(upstream, context, NisServer::EventType::Write);

// Now reads/writes to 'upstream' are non-blocking and cooperative
upstream << "GET /data HTTP/1.1\r\nHost: backend.local\r\n\r\n" << std::flush;

std::string line;
std::getline(upstream, line);  // yields if data not ready
```

### RAII Helpers

| Class | Purpose |
|-------|---------|
| `AsyncStream` | Register an additional `SocketStream` with the event loop |
| `AsyncSocket` | Register an additional raw `Socket` |
| `AsyncSharedSocket` | Register a socket shared across connections (e.g., connection pool) |

---

## PyntControl

A simple control port that triggers a hard stop when any connection is made:

```cpp
NisServer::PyntControl control(server);
server.listen(TASock::ServerInfo{9090}, control);
```

Connect to port 9090 from anywhere to shut down the server.

---

## Complete Example: Multi-Port Server

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseServer/PyntControl.h"
#include "NisseHTTP/HTTPHandler.h"
#include <ThorsSocket/Server.h>

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
namespace TASock    = ThorsAnvil::ThorsSocket;

class MyServer : public NisServer::NisseServer
{
    NisHttp::HTTPHandler    http;
    NisServer::PyntControl  control;

public:
    MyServer(int port)
        : control(*this)
    {
        http.addPath(NisHttp::Method::GET, "/hello/{name}",
            [](NisHttp::Request const& req, NisHttp::Response& res) {
                std::string name = req.variables()["name"];
                std::string body = "Hello, " + name + "!";
                res.body(body.size()) << body;
                return true;
            });

        listen(TASock::ServerInfo{port}, http);
        listen(TASock::ServerInfo{port + 2}, control);  // control port
    }
};

int main()
{
    MyServer server(8080);
    server.run();
}
```
