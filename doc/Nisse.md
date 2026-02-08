# Nisse Developer Guide

This document is the in-depth developer reference for the **Nisse** server framework. It covers the architecture, class responsibilities, data-flow, threading model, and extension points across the two core libraries — `NisseServer` and `NisseHTTP`.

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [NisseServer Library](#nisseserver-library)
   - [NisseServer](#nisseserver)
   - [Pynt](#pynt)
   - [Context](#context)
   - [EventHandler](#eventhandler)
   - [JobQueue](#jobqueue)
   - [Store](#store)
   - [TimerAction](#timeraction)
   - [NisseUtil (Type Aliases)](#nisseutil-type-aliases)
   - [EventHandlerLibEvent (RAII Wrappers)](#eventhandlerlibevent-raii-wrappers)
3. [NisseHTTP Library](#nissehttp-library)
   - [PyntHTTP](#pynthttp)
   - [HTTPHandler](#httphandler)
   - [Request](#request)
   - [Response](#response)
   - [URL](#url)
   - [HeaderRequest / HeaderResponse](#headerrequest--headerresponse)
   - [HeaderPassThrough](#headerpassthrough)
   - [StreamInput / StreamOutput](#streaminput--streamoutput)
   - [PathMatcher](#pathmatcher)
   - [Util (Enums, Types, Helpers)](#util-enums-types-helpers)
   - [Client-Side HTTP (ClientStream, ClientRequest, ClientResponse)](#client-side-http)
   - [PyntHTTPControl](#pynthttpcontrol)
4. [Threading & Coroutine Model](#threading--coroutine-model)
5. [Connection Lifecycle](#connection-lifecycle)
6. [State Management & Thread Safety](#state-management--thread-safety)
7. [Extending Nisse](#extending-nisse)
   - [Writing a Custom Pynt](#writing-a-custom-pynt)
   - [Writing an HTTPHandler-Based Service](#writing-an-httphandler-based-service)
   - [Registering Additional Sockets (Async I/O)](#registering-additional-sockets-async-io)
   - [Using Timers](#using-timers)
8. [Header-Only Mode](#header-only-mode)
9. [Namespace Reference](#namespace-reference)
10. [File Index](#file-index)

---

## Architecture Overview

Nisse is structured as two layered libraries:

```
┌──────────────────────────────────────────────────┐
│                  User Application                 │
│         (HelloWorld, WebServer, etc.)             │
├──────────────────────────────────────────────────┤
│                   NisseHTTP                       │
│   PyntHTTP · HTTPHandler · Request · Response     │
│   URL · Headers · PathMatcher · Streams           │
├──────────────────────────────────────────────────┤
│                  NisseServer                      │
│   NisseServer · Pynt · Context · EventHandler     │
│   JobQueue · Store · EventHandlerLibEvent         │
├──────────────────────────────────────────────────┤
│              External Dependencies                │
│   ThorsSocket · boost::coroutine2 · libEvent      │
└──────────────────────────────────────────────────┘
```

**NisseServer** provides the transport-agnostic core: event loop, thread pool, coroutine-based cooperative multitasking, and a protocol-agnostic `Pynt` interface.

**NisseHTTP** builds on top to provide HTTP/1.x request parsing, response generation, URL routing, and client-side HTTP utilities.

---

## NisseServer Library

**Namespace:** `ThorsAnvil::Nisse::Server`
**Directory:** `src/NisseServer/`

### NisseServer

**Files:** `NisseServer.h`, `NisseServer.cpp`

The central server object. It owns and coordinates three subsystems:

| Member          | Type           | Role                                            |
|-----------------|----------------|-------------------------------------------------|
| `jobQueue`      | `JobQueue`     | Thread pool that executes connection work items  |
| `store`         | `Store`        | Holds all live connection state (file-descriptor indexed) |
| `eventHandler`  | `EventHandler` | libEvent wrapper that dispatches I/O readiness events |

#### Public API

```cpp
NisseServer(std::size_t workerCount = 1);
```
Creates the server with `workerCount` worker threads in the pool.

```cpp
void run(std::function<void()>&& notice = [](){});
```
Starts the event loop. The optional `notice` callback fires just before the loop begins (useful for signalling test harnesses that the server is ready). This call blocks until the server is stopped.

```cpp
void stopSoft();
```
Initiates a graceful shutdown. The server stops accepting new connections but waits for all active connections to complete before exiting the event loop.

```cpp
void stopHard();
```
Initiates an immediate shutdown. The event loop exits on the next timer tick regardless of active connections.

```cpp
void listen(TASock::ServerInit&& listenerInit, Pynt& pynt);
```
Opens a listening socket and associates it with a `Pynt` protocol handler. New connections on this socket are dispatched to `pynt.handleRequest()`. Call multiple times to listen on multiple ports.

- `TASock::ServerInfo{port}` — plain TCP on `port`
- `TASock::SServerInfo{port, ctx}` — TLS on `port` with an `SSLctx`

```cpp
template<typename T, typename rep>
int addTimer(std::chrono::duration<T, rep> time, TimerAction& action);
```
Registers a repeating timer. Returns a timer ID. `action.handleRequest(timerId)` is called each time the timer fires.

#### Internal Methods

- `createStreamJob(StreamData&)` — builds the coroutine that manages a single client connection's lifecycle. Sets up read/write yield lambdas, performs deferred SSL handshake, then loops calling `pynt->handleRequest()` until `PyntResult::Done`.
- `createAcceptJob(ServerData&)` — builds the coroutine for a listening socket. Loops accepting new connections and registers each via `eventHandler.add()`.

---

### Pynt

**Files:** `Pynt.h`, `Pynt.cpp`

Abstract base class for protocol handlers. A `Pynt` is the unit of protocol logic you plug into the server.

```cpp
class Pynt {
public:
    virtual ~Pynt();
    virtual PyntResult handleRequest(TASock::SocketStream& stream, Context& context) = 0;
};
```

**Parameters:**
- `stream` — a `std::iostream` wrapping the client socket. Reads and writes to this stream are transparently non-blocking; the coroutine yields when I/O would block.
- `context` — provides access to the server's async registration facilities (for opening additional sockets, etc.).

**Return value:**
- `PyntResult::Done` — connection is finished; the server closes the socket.
- `PyntResult::More` — this request is complete but the connection stays open for the next request (e.g. HTTP keep-alive). The server will call `handleRequest()` again when data arrives.

---

### Context

**Files:** `Context.h`, `Context.cpp`

Provides the interface for a connection handler to interact with the server's async I/O system. Passed into every `Pynt::handleRequest()` call.

#### Key Methods

```cpp
void registerOwnedSocketStream(TASock::SocketStream& stream, EventType initialWait);
void unregisterOwnedSocketStream(TASock::SocketStream& stream);
```
Register an **additional** socket stream (not the main client stream — that one is already managed) with the event loop. The coroutine will yield when I/O on this socket blocks. The `initialWait` determines whether to initially listen for read or write readiness.

```cpp
void registerOwnedSocket(TASock::Socket& socket, EventType initialWait);
void unregisterOwnedSocket(TASock::Socket& socket);
```
Same as above but for raw sockets (not wrapped in a `SocketStream`).

```cpp
static void registerSharedSocket(NisseServer& server, TASock::Socket& socket);
static void unregisterSharedSocket(NisseServer& server, TASock::Socket& socket);
```
Register a socket that is **shared** across multiple connections (e.g., a connection-pool pipe). Multiple coroutines can wait on the same shared socket; they are queued and resumed in FIFO order.

```cpp
Yield& getYield();
```
Direct access to the coroutine yield object. Needed only for advanced custom yielding.

#### RAII Helpers

Three RAII wrappers automate registration/unregistration:

| Class               | Wraps                        | Usage                                    |
|---------------------|------------------------------|------------------------------------------|
| `AsyncStream`       | `SocketStream` + `Context`   | Additional async stream within a handler |
| `AsyncSocket`       | `Socket` + `Context`         | Additional async raw socket              |
| `AsyncSharedSocket` | `Socket` + `NisseServer`     | Shared socket (e.g. connection pool)     |

Construct the RAII object to register; it unregisters on destruction.

---

### EventHandler

**Files:** `EventHandler.h`, `EventHandler.cpp`

A C++ wrapper around libEvent. Receives I/O readiness notifications and dispatches work to the `JobQueue`.

#### Responsibilities

1. Maintains libEvent `event_base` and per-fd `event` objects.
2. On I/O readiness, enqueues a job on the `JobQueue` to resume the connection's coroutine.
3. After coroutine yields, interprets the yield state (`RestoreRead`, `RestoreWrite`, `WaitForMore`, `Remove`) and requests the corresponding state change via the `Store`.
4. Runs a periodic internal timer (10ms) that calls `Store::processUpdateRequest()` on the main thread to apply queued state mutations.
5. Manages soft/hard stop logic.

#### Yield State Machine

When a coroutine yields, the `EventHandler::addJob()` lambda inspects the `TaskYieldState`:

| State           | Meaning                                          | Action                                                  |
|-----------------|--------------------------------------------------|---------------------------------------------------------|
| `RestoreRead`   | Blocked on read; resume when readable            | Re-register read event for this fd                      |
| `RestoreWrite`  | Blocked on write; resume when writable           | Re-register write event for this fd                     |
| `WaitForMore`   | Request complete; wait for next request           | Re-register read event; decrement active count          |
| `Remove`        | Connection done; clean up                        | Remove fd from store; decrement active count            |

#### Event Dispatch (Visitor Pattern)

The `Store` holds a `std::variant<ServerData, StreamData, OwnedFD, SharedFD, TimerData>` per fd. The `EventHandler::eventAction()` method uses `std::visit` with `ApplyEvent` to call the correct handler:

- `handleServerEvent` — a listening socket has a pending connection → resume accept coroutine.
- `handleStreamEvent` — a client socket is readable/writable → resume stream coroutine (after `checkFileDescriptorOK`).
- `handleLinkStreamEvent` — an **owned** secondary fd is ready → resume the **owner's** coroutine.
- `handlePipeStreamEvent` — a **shared** fd is ready → resume the next queued coroutine.
- `handleTimerEvent` — a timer fired → call `TimerAction::handleRequest()`.

---

### JobQueue

**Files:** `JobQueue.h`, `JobQueue.cpp`

A simple thread pool backed by `std::thread`, `std::mutex`, and `std::condition_variable`.

```cpp
JobQueue(std::size_t workerCount);  // Spawns workerCount threads
~JobQueue();                         // Calls stop() and joins all threads

void addJob(Work&& action);          // Enqueues a std::function<void()>
void stop();                         // Signals all workers to exit
```

Each worker loops calling `getNextJob()` which blocks on the condition variable until work is available or `finished` is set.

---

### Store

**Files:** `Store.h`, `Store.cpp`

The central state repository. Every active file descriptor (listening sockets, client streams, owned fds, shared fds, timers) has an entry in a `std::map<int, StoreData>`.

```cpp
using StoreData = std::variant<ServerData, StreamData, OwnedFD, SharedFD, TimerData>;
```

#### Thread Safety Model

**Only the main thread mutates the `data` map.** Worker threads request mutations by calling `requestChange()`, which enqueues a `StateUpdate` variant under a mutex. The main thread calls `processUpdateRequest()` on each timer tick (every ~10ms) to drain the queue and apply all changes.

This avoids contention on the main data structure and ensures the libEvent thread is the only one modifying event registrations.

#### Data Structures per FD

| Type         | Contents                                                    |
|--------------|-------------------------------------------------------------|
| `ServerData` | `TASock::Server`, `CoRoutine`, read `Event`, `Pynt*`       |
| `StreamData` | `TASock::SocketStream`, `CoRoutine`, read+write `Event`, `Pynt*` |
| `OwnedFD`   | Pointer to owner's `CoRoutine`, read+write `Event`          |
| `SharedFD`  | Queues of waiting `CoRoutine*` (read/write), read+write `Event` |
| `TimerData`  | Timer ID, interval, `TimerAction*`, `EventHandler*`, timer `Event` |

#### State Update Types

| Update Type                  | Action                                              |
|------------------------------|-----------------------------------------------------|
| `StateUpdateCreateServer`    | Insert a new listening socket + activate read event  |
| `StateUpdateCreateStream`    | Insert a new client stream + activate read event     |
| `StateUpdateCreateOwnedFD`   | Link a secondary fd to its owner's coroutine         |
| `StateUpdateCreateSharedFD`  | Create a shared fd entry with empty wait queues      |
| `StateUpdateCreateTimer`     | Create a timer entry and start the timer             |
| `StateUpdateExternallClosed` | Mark socket as externally closed, remove entry       |
| `StateUpdateRemove`          | Remove an fd from the store                          |
| `StateUpdateRestoreRead`     | Re-register read event for an fd                     |
| `StateUpdateRestoreWrite`    | Re-register write event for an fd                    |

---

### TimerAction

**File:** `TimerAction.h`

Abstract base for timer callbacks:

```cpp
class TimerAction {
public:
    virtual ~TimerAction() {}
    virtual void handleRequest(int timerId) = 0;
};
```

Register via `NisseServer::addTimer()`. The `handleRequest()` method is called on the main event-loop thread each time the timer fires.

---

### NisseUtil (Type Aliases)

**File:** `NisseUtil.h`

Defines the fundamental types used across the server:

```cpp
enum class TaskYieldState { RestoreRead, RestoreWrite, WaitForMore, Remove };

struct TaskYieldAction {
    TaskYieldState state;
    int            fd;
};

using CoRoutine     = boost::coroutines2::coroutine<TaskYieldAction>::pull_type;
using Yield         = boost::coroutines2::coroutine<TaskYieldAction>::push_type;
using ServerTask    = std::function<void(TASock::Server& stream, Yield& yield)>;
using StreamTask    = std::function<void(TASock::SocketStream& stream, Yield& yield)>;
using ServerCreator = std::function<CoRoutine(ServerData&)>;
using StreamCreator = std::function<CoRoutine(StreamData&)>;
```

The coroutine yields a `TaskYieldAction` to communicate its intent to the `EventHandler`.

---

### EventHandlerLibEvent (RAII Wrappers)

**File:** `EventHandlerLibEvent.h`

RAII wrappers for libEvent primitives:

- **`EventBase`** — wraps `event_base*`. Non-copyable, non-movable. Provides `run()` (enters the event loop) and `loopBreak()` (exits it). Also exposes feature detection (`isFeatureEnabled`) to check, for example, whether `epoll` supports file descriptors.
- **`Event`** — wraps `event*`. Move-only. Provides `add()` (activates the event) and `add(int microseconds)` (timer variant).
- **`EventType`** — enum: `Read`, `Write` (maps to `EV_READ`, `EV_WRITE`).
- **`Feature`** — enum: `FileReadWriteEvent` (false on `epoll` systems).

---

## NisseHTTP Library

**Namespace:** `ThorsAnvil::Nisse::HTTP`
**Directory:** `src/NisseHTTP/`

### PyntHTTP

**Files:** `PyntHTTP.h`, `PyntHTTP.cpp`

An HTTP-specific implementation of `Pynt`. It parses the raw socket stream into `Request` and `Response` objects, then delegates to the pure virtual:

```cpp
class PyntHTTP : public Server::Pynt {
public:
    Server::PyntResult handleRequest(TASock::SocketStream& stream, Server::Context& context) override;
    virtual void processRequest(Request& request, Response& response) = 0;
};
```

**Behavior:**
1. Constructs a `Request` by parsing the stream.
2. If the request is invalid, sends a 400 response and returns `PyntResult::Done`.
3. Otherwise, constructs a `Response` (default 200) and calls `processRequest()`.
4. Returns `PyntResult::More` (HTTP keep-alive).

---

### HTTPHandler

**Files:** `HTTPHandler.h`, `HTTPHandler.cpp`

A concrete `PyntHTTP` subclass that provides URL-pattern-based routing.

#### Registering Routes

```cpp
void addPath(std::string const& path, HTTPAction&& action,
             HTTPValidate&& val = [](Request const&){ return true; });
void addPath(MethodChoice method, std::string const& path,
             HTTPAction&& action, HTTPValidate&& val = ...);
```

- `path` — a URL pattern with `{name}` placeholders (e.g. `/user/{id}/profile`).
- `action` — `std::function<bool(Request const&, Response&)>` called when the path matches.
- `val` — optional validation function called before `action`; returning `false` sends a 400.
- `method` — either a specific `Method` enum value or `All::Method` for any method.

```cpp
void remPath(std::string const& path);
void remPath(MethodChoice method, std::string const& path);
```

Removes a previously registered route.

#### Request Processing Pipeline

When `processRequest()` is called:

1. Normalize the URL path.
2. Delegate to `PathMatcher::findMatch()`.
3. If a match is found:
   - Populate `request.variables()` with: HTTP headers, URL query parameters, path-captured variables, and form body variables (if `content-type: application/x-www-form-urlencoded`).
   - Call the validation function; send 400 on failure.
   - Call the user action.
4. If no match: send 404.

#### HTTPAction Signature

```cpp
using HTTPAction = std::function<bool(Request const& request, Response& response)>;
```

Return `true` if the action handled the request, `false` to fall through.

#### HTTPValidate Signature

```cpp
using HTTPValidate = std::function<bool(Request const& request)>;
```

Return `true` to allow the request, `false` to reject with 400.

---

### Request

**Files:** `Request.h`, `Request.cpp`

Represents a parsed HTTP request.

#### Construction

```cpp
Request(Server::Context& context, std::string_view proto, std::istream& stream);
```

Parsing happens during construction:
1. Read the first line (method, path, version).
2. Read headers into `HeaderRequest`.
3. Build the `URL` object from the `Host` header and request path.
4. Create the body `StreamInput` based on `Content-Length` or `Transfer-Encoding: chunked`.

If any parsing step fails, `failResponse` is populated and `isValidRequest()` returns `false`.

#### Public API

| Method               | Returns                    | Description                                      |
|----------------------|----------------------------|--------------------------------------------------|
| `getContext()`       | `Server::Context&`         | The server context for this connection            |
| `getVersion()`      | `Version`                  | HTTP version (HTTP1_0, HTTP1_1, HTTP2, HTTP3)    |
| `getMethod()`       | `Method`                   | HTTP method (GET, POST, PUT, etc.)               |
| `getUrl()`          | `URL const&`               | Parsed URL object                                 |
| `httpRawRequest()`  | `std::string_view`         | The raw first line of the request                 |
| `headers()`         | `HeaderRequest const&`     | Request headers                                   |
| `trailers()`        | `HeaderRequest const&`     | HTTP trailers (populated after body is fully read) |
| `failHeader()`      | `HeaderResponse const&`    | Error details if request parsing failed           |
| `isValidRequest()`  | `bool`                     | `true` if request parsed successfully             |
| `variables()`       | `RequestVariables&`        | Key-value map populated by `HTTPHandler`          |
| `body()`            | `std::istream&`            | Input stream for the request body                 |
| `preloadStreamIntoBuffer()` | `std::string_view`  | Reads entire body into memory, returns view       |

The request is streamable: `stream << request` outputs the first line and headers in HTTP format.

---

### Response

**Files:** `Response.h`, `Response.cpp`

Represents an HTTP response being built.

#### Construction

```cpp
Response(std::ostream& stream, Version version, int code = 200);
```

The status line and headers are **not** sent until needed (lazy flushing).

#### Public API

```cpp
void setStatus(int code);
```
Changes the response status code. Must be called before headers are sent.

```cpp
void addHeaders(Header const& headers);
```
Sends the status line (if not yet sent) followed by the provided headers. `Header` is a variant accepting either `HeaderResponse` or `HeaderPassThrough`.

```cpp
std::ostream& body(BodyEncoding encoding);
```
Sends the status line and encoding header, then returns an output stream for the body. `BodyEncoding` is a variant:
- `std::size_t` or `std::streamsize` — fixed content-length
- `Encoding::Chunked` — chunked transfer encoding

```cpp
void error(int code, std::string_view errorMessage);
```
Convenience: sets the status code and adds an `Error` header.

```cpp
friend std::istream& operator>>(std::istream& stream, Response& response);
```
Reads a response status line from another server (used in reverse-proxy scenarios).

#### Lifecycle

If neither `addHeaders()` nor `body()` is called before destruction, the destructor sends a minimal response with `content-length: 0`.

The destructor also logs the response time (from construction to destruction).

---

### URL

**Files:** `URL.h`, `URL.cpp`

Parses and stores a URL with accessors for each component:

```
http://localhost:53/status?name=ryan#234
│       │          │      │          │
protocol origin    path   query      hash
        │
        host (hostname:port)
```

| Accessor      | Example Return        |
|---------------|-----------------------|
| `href()`      | `http://localhost:53/status?name=ryan#234` |
| `protocol()`  | `http:`               |
| `origin()`    | `http://localhost:53` |
| `host()`      | `localhost:53`        |
| `hostname()`  | `localhost`           |
| `port()`      | `53`                  |
| `pathname()`  | `/status`             |
| `query()`     | `?name=ryan`          |
| `hash()`      | `#234`                |
| `param("name")` | `ryan`              |

Supports copy, move, swap, and equality comparison.

---

### HeaderRequest / HeaderResponse

**Files:** `HeaderRequest.h`, `HeaderResponse.h` + `.cpp` files

Two header container types:

**`HeaderRequest`** — stores incoming request headers as a `std::map<std::string, std::vector<std::string>>`. Supports multiple values per header name (required for headers like `Set-Cookie`).

```cpp
bool hasHeader(std::string_view header) const;
std::vector<std::string> const& getHeader(std::string const& header) const;
void add(std::string_view header, std::string_view value);
```

**`HeaderResponse`** — stores outgoing response headers as a `std::map<std::string, std::string>`. Single value per header.

```cpp
bool empty() const;
void add(std::string_view header, std::string_view value);
```

Both are streamable via `operator<<`.

---

### HeaderPassThrough

**Files:** `HeaderPassThrough.h`, `HeaderPassThrough.cpp`

Used in reverse-proxy scenarios. Reads headers from an upstream response and writes them directly to the downstream response without full parsing. Detects `Content-Length` and `Transfer-Encoding` to determine body encoding.

```cpp
HeaderPassThrough(std::istream& stream);
BodyEncoding getEncoding() const;
```

Streamable: `response.addHeaders(headerPassThrough)` writes the headers through.

---

### StreamInput / StreamOutput

**Files:** `StreamInput.h`, `StreamOutput.h` + `.cpp` files

Custom `std::streambuf` implementations that handle HTTP body encoding/decoding transparently.

**`StreamBufInput`** / **`StreamInput`** — reads the request body. Supports:
- Fixed `Content-Length`: reads exactly N bytes then signals EOF.
- `Transfer-Encoding: chunked`: reads chunk headers, decodes chunks, reads trailers on completion.
- Provides `preloadStreamIntoBuffer()` to read the entire body into memory.

**`StreamBufOutput`** / **`StreamOutput`** — writes the response body. Supports:
- Fixed content-length: writes exactly N bytes.
- Chunked: wraps output in HTTP chunk framing.
- Handles proper flushing and chunk termination on destruction.

---

### PathMatcher

**Files:** `PathMatcher.h`, `PathMatcher.cpp`

Matches URL paths against registered patterns with `{name}` capture groups.

#### Pattern Syntax

- Literal segments: `/api/v1/users`
- Capture groups: `/user/{id}` — matches any URL-safe characters except `/`
- Mixed: `/content/{file}.html` — captures `file` between `/content/` and `.html`
- Multiple captures: `/person/findByName/{first}/{last}`

Captured values are URL-decoded and returned in a `Match` map (`std::map<std::string, std::string>`).

#### Method Filtering

Routes can be restricted to a specific HTTP method or match all methods:

```cpp
using MethodChoice = std::variant<Method, All>;
```

#### Internal Representation

Each registered path is decomposed into:
- `matchSections` — the literal string segments between captures
- `names` — the names of the capture groups
- `method` — the method filter
- `action` — function pointer + data (structured this way to avoid issues with `dlclose()` on dynamically loaded plugins)

---

### Util (Enums, Types, Helpers)

**File:** `Util.h`, `Util.cpp`

Central definitions used across NisseHTTP:

```cpp
enum class Version  { HTTP1_0, HTTP1_1, HTTP2, HTTP3, Unknown };
enum class Method   { GET, HEAD, OPTIONS, TRACE, PUT, DELETER, POST, PATCH, CONNECT, Other };
enum class All      { Method };     // Matches any HTTP method
enum class Encoding { Chunked };

using MethodChoice = std::variant<Method, All>;
using BodyEncoding = std::variant<std::size_t, std::streamsize, Encoding>;
```

**`StatusCode`** — pairs an integer code with its standard message text.

**`StandardStatusCodeMap`** — singleton lookup table for standard HTTP status codes.

**`RequestVariables`** — a `std::map<std::string, std::string>` wrapper used to carry path captures, query parameters, headers, and form fields as a unified variable set. Provides `exists()`, `operator[]` (returns empty string on miss), `insert_or_assign()`, and iteration.

**`Header`** — a variant type `std::variant<HeaderResponse const&, HeaderPassThrough const&>` used where either header type is acceptable.

---

### Client-Side HTTP

**Files:** `ClientStream.h`, `ClientRequest.h`, `ClientResponse.h` + `.cpp` files

Utilities for making outbound HTTP requests (e.g. from a reverse proxy).

**`ClientStream`** — opens a TLS socket connection to a URL:

```cpp
ClientStream(std::string const& url);
operator std::ostream&();
operator std::istream&();
```

Parses the host and port from the URL, establishes an SSL connection.

**`ClientRequest`** — builds and sends an HTTP request:

```cpp
ClientRequest(std::ostream& stream, std::string url,
              Method method = Method::GET, Version version = Version::HTTP1_1);

void addHeaders(Header const& headers);
std::ostream& body(BodyEncoding encoding);
void flushRequest();
```

The first line and `Host` header are sent lazily. The destructor calls `flushRequest()`.

**`ClientResponse`** — reads and parses an HTTP response:

```cpp
ClientResponse(std::istream& stream);
std::size_t getContentSize() const;
```

Parses the status line and headers from the stream. Streamable via `operator<<`.

---

### PyntHTTPControl

**Files:** `PyntHTTPControl.h`, `PyntHTTPControl.cpp`

An HTTP-based server control endpoint. Accepts query parameter `command` with values:

| Command    | Action                                            |
|------------|---------------------------------------------------|
| `stophard` | Calls `NisseServer::stopHard()` — immediate stop |
| `stopsoft` | Calls `NisseServer::stopSoft()` — graceful stop  |
| `ping`     | No-op; the request itself serves as a health check |

Usage: `GET /control?command=stopsoft`

There is also a simpler `PyntControl` (in `NisseServer`) that triggers a hard stop on any connection — useful as a raw TCP control port.

---

## Threading & Coroutine Model

Nisse uses a hybrid cooperative/preemptive threading model:

```
Main Thread                    Worker Threads (N)
───────────                    ──────────────────
libEvent loop                  JobQueue consumers
  │                               │
  ├─ I/O readiness event          │
  │  └─ Enqueue job ──────────►   ├─ Resume coroutine
  │                               │   ├─ Read/write on stream
  ├─ Timer tick (10ms)            │   ├─ I/O would block?
  │  └─ processUpdateRequest()    │   │  └─ Yield ──► (coroutine suspended)
  │     └─ Apply state changes    │   │     └─ Enqueue state change request
  │        └─ Re-register events  │   └─ Handler returns
  │                               │      └─ Enqueue state change request
  └─ (loop continues)            └─ (pick up next job)
```

**Key invariant:** All mutations to the `Store` data map and libEvent registrations happen on the main thread (via `processUpdateRequest()`). Worker threads only enqueue `StateUpdate` requests.

**Coroutine mechanics:** Each connection gets a `boost::coroutines2` pull-type coroutine. When I/O would block, ThorsSocket calls the registered yield function, which pushes a `TaskYieldAction` and suspends the coroutine. The worker thread then returns to the job queue. When the event loop detects readiness, it enqueues a new job that resumes the coroutine from where it left off.

This means **handler code reads as synchronous** — no callbacks, no futures, no explicit state machines. Just standard `stream >> data` and `stream << data`.

---

## Connection Lifecycle

```
1. Server socket receives connection
   └─ Accept coroutine creates SocketStream
      └─ EventHandler::add(stream, ...) enqueues StateUpdateCreateStream

2. Main thread timer fires
   └─ Store::processUpdateRequest()
      └─ Creates StreamData, stores coroutine, activates read Event

3. Client sends data → libEvent triggers read event
   └─ EventHandler::handleStreamEvent()
      └─ jobQueue.addJob() → resumes coroutine

4. Coroutine runs Pynt::handleRequest()
   ├─ Read from stream → may yield (RestoreRead)
   ├─ Write to stream  → may yield (RestoreWrite)
   └─ Returns PyntResult

5a. PyntResult::More → yield(WaitForMore)
    └─ Re-register read event, wait for next request → go to step 3

5b. PyntResult::Done → yield(Remove)
    └─ Remove from Store, close socket
```

---

## State Management & Thread Safety

The `Store` uses a **command queue pattern** for thread safety:

1. **Worker threads** call `store.requestChange(update)` which locks `updateMutex` and pushes to the `updates` vector.
2. **Main thread** (via timer callback) calls `store.processUpdateRequest()` which locks `updateMutex`, drains the vector, and applies each update sequentially.
3. The `data` map is only ever modified by the main thread, so no locking is needed for reads during event dispatch (which also runs on the main thread).

The `openStreamCount` is incremented/decremented by worker threads but is only read by the main thread during soft-stop checks, so the slight race is acceptable (the timer will re-check).

---

## Extending Nisse

### Writing a Custom Pynt

For a non-HTTP protocol, implement `Pynt` directly:

```cpp
#include "NisseServer/Pynt.h"

class MyProtocol : public ThorsAnvil::Nisse::Server::Pynt {
public:
    PyntResult handleRequest(TASock::SocketStream& stream, Context& context) override {
        std::string line;
        std::getline(stream, line);          // Reads asynchronously
        stream << "Echo: " << line << "\n";  // Writes asynchronously
        return PyntResult::More;             // Keep connection open
    }
};
```

### Writing an HTTPHandler-Based Service

The most common pattern — subclass `NisseServer`, compose an `HTTPHandler`, register routes:

```cpp
#include "NisseServer/NisseServer.h"
#include "NisseHTTP/HTTPHandler.h"

namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
namespace TASock    = ThorsAnvil::ThorsSocket;

class MyApp : public NisServer::NisseServer {
    NisHttp::HTTPHandler http;

public:
    MyApp(int port) {
        http.addPath(NisHttp::Method::GET, "/hello/{name}",
            [](NisHttp::Request const& req, NisHttp::Response& res) {
                std::string name = req.variables()["name"];
                std::string body = "Hello, " + name + "!";
                res.body(body.size()) << body;
                return true;
            });

        http.addPath(NisHttp::Method::POST, "/data",
            [](NisHttp::Request const& req, NisHttp::Response& res) {
                // Read the request body
                std::string input;
                std::getline(req.body(), input);

                res.body(NisHttp::Encoding::Chunked) << "Received: " << input;
                return true;
            });

        listen(TASock::ServerInfo{port}, http);
    }
};

int main() {
    MyApp server(8080);
    server.run();
}
```

### Registering Additional Sockets (Async I/O)

Within a handler, you can open additional connections (to databases, upstream servers, etc.) and make them participate in the async event loop:

```cpp
// Inside an HTTPHandler action lambda:
[](NisHttp::Request const& req, NisHttp::Response& res) {
    // Open a connection to an upstream server
    TASock::SocketStream upstream{TASock::Socket{
        TASock::SocketInfo{"backend.local", 9090},
        TASock::Blocking::No
    }};

    // Register with the event loop (RAII — unregisters on scope exit)
    NisServer::AsyncStream async(upstream, req.getContext(), NisServer::EventType::Write);

    // Now reads/writes to 'upstream' are non-blocking and cooperative
    upstream << "GET /api/data HTTP/1.1\r\nHost: backend.local\r\n\r\n" << std::flush;

    std::string line;
    std::getline(upstream, line);  // Yields if data not ready

    res.body(NisHttp::Encoding::Chunked) << "Upstream says: " << line;
    return true;
}
```

For sockets shared across connections (e.g. a connection pool), use `AsyncSharedSocket`. See the `MongoRest` example for a complete connection-pool implementation using a pipe-based semaphore pattern.

### Using Timers

```cpp
class MyTimedServer : public NisServer::NisseServer {
    struct MyTimer : public NisServer::TimerAction {
        void handleRequest(int timerId) override {
            std::cout << "Timer " << timerId << " fired!\n";
        }
    };

    MyTimer timer;

public:
    MyTimedServer(int port) {
        // Fire every 5 seconds
        addTimer(std::chrono::seconds(5), timer);
        // ... listen() ...
    }
};
```

---

## Header-Only Mode

Nisse supports header-only compilation when `NISSE_HEADER_ONLY` is defined to `1`. In this mode:

- Implementation files use `.source` extension instead of `.cpp`
- Headers conditionally `#include` the `.source` files
- The `NISSE_HEADER_ONLY_INCLUDE` macro expands to `inline` to avoid ODR violations

This is useful for plugin architectures where the library is compiled into shared objects loaded via `dlopen`/`dlclose`.

---

## Namespace Reference

| Namespace                        | Library      | Contents                          |
|----------------------------------|--------------|-----------------------------------|
| `ThorsAnvil::Nisse::Server`     | NisseServer  | Core server, event loop, store    |
| `ThorsAnvil::Nisse::HTTP`       | NisseHTTP    | HTTP protocol, routing, streams   |

Commonly used namespace aliases in application code:

```cpp
namespace TASock    = ThorsAnvil::ThorsSocket;
namespace NisServer = ThorsAnvil::Nisse::Server;
namespace NisHttp   = ThorsAnvil::Nisse::HTTP;
```

---

## File Index

### NisseServer (`src/NisseServer/`)

| File                       | Description                                            |
|----------------------------|--------------------------------------------------------|
| `NisseServer.h/.cpp`      | Main server class — owns JobQueue, Store, EventHandler |
| `Pynt.h/.cpp`             | Abstract protocol handler interface                    |
| `PyntControl.h/.cpp`      | Simple TCP control port (hard stop on connect)         |
| `Context.h/.cpp`          | Per-connection context, async socket registration      |
| `EventHandler.h/.cpp`     | libEvent wrapper, I/O dispatch, yield state machine    |
| `EventHandlerLibEvent.h`  | RAII wrappers for `event_base` and `event`             |
| `JobQueue.h/.cpp`         | Thread pool                                            |
| `Store.h/.cpp`            | State repository, command-queue mutations               |
| `NisseUtil.h`             | Core type aliases (CoRoutine, Yield, TaskYieldState)   |
| `TimerAction.h`           | Abstract timer callback interface                      |

### NisseHTTP (`src/NisseHTTP/`)

| File                        | Description                                           |
|-----------------------------|-------------------------------------------------------|
| `PyntHTTP.h/.cpp`          | HTTP implementation of Pynt                           |
| `PyntHTTPControl.h/.cpp`   | HTTP control endpoint (stophard/stopsoft/ping)         |
| `HTTPHandler.h/.cpp`       | URL-pattern-based route dispatcher                    |
| `Request.h/.cpp`           | HTTP request parser                                   |
| `Response.h/.cpp`          | HTTP response builder                                 |
| `URL.h/.cpp`               | URL parser with component accessors                   |
| `HeaderRequest.h/.cpp`     | Incoming header container (multi-value)                |
| `HeaderResponse.h/.cpp`    | Outgoing header container (single-value)               |
| `HeaderPassThrough.h/.cpp` | Proxy-style header forwarding                         |
| `HeaderStreamOperator.tpp` | Variant stream operator for Header type               |
| `StreamInput.h/.cpp`       | Input streambuf (content-length / chunked decoding)   |
| `StreamOutput.h/.cpp`      | Output streambuf (content-length / chunked encoding)  |
| `PathMatcher.h/.cpp`       | URL pattern matching with captures                    |
| `Util.h/.cpp`              | Enums, StatusCode, RequestVariables, BodyEncoding     |
| `ClientStream.h/.cpp`      | Outbound TLS socket connection                        |
| `ClientRequest.h/.cpp`     | Outbound HTTP request builder                         |
| `ClientResponse.h/.cpp`    | Inbound HTTP response parser                          |
