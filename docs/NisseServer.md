[Home](index.html) | [Internal Documentation](internal/NisseServer.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# NisseServer API

An event-driven, coroutine-based C++ server framework. Write synchronous-looking request handlers while the framework handles non-blocking async I/O, thread pools, and connection management behind the scenes.

**Note**: This is for base level socket handling and does not provide any protocol specific handling. Please see NisseHTTP for HTTP specific protocol handling.

**Namespace:** `ThorsAnvil::Nisse::Server`

---

## Quick Start

### Makefile
```Makefile
CXXFLAGS        = -std=c++20

LDLIBS          = -lThorsSocket -lNisse -lboost_coroutine -lboost_context -levent

all:            MyServer
```
### MyServer.cpp

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseServer/Pynt.h"
#include "NisseServer/PyntControl.h"
#include "ThorsSocket/SocketStream.h"

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
        std::getline(stream, line);                     // reads asynchronously
        stream << "Echo: " << line << "\n";             // writes asynchronously
        return NisServer::PyntResult::More;             // keep connection open
    }
};

int main()
{
    NisServer::NisseServer server;                      // A basic server
    NisServer::PyntControl control(server);             // Server simple control. Ability to stop server.
    EchoProtocol echo;

    // Add Listeners:
    server.listen(TASock::ServerInfo{8080}, echo);
    server.listen(TASock::ServerInfo{9090}, control);

    // Start server
    std::cout << "Listening on port: 8080\n";
    server.run();  // blocks until stopped
}
```

### Testing Server

Bash client using line based protocol:

```bash
> make
> ./MyServer &

# nc => netcat
> echo "Your raw text here" | nc localhost 8080
# Expected Output
# Echo: Your raw text here

# Stop server with:
> echo "Stop" | nc localhost 9090
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
