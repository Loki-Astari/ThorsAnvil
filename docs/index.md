# ThorsAnvil

A set of modern C++20 libraries for writing interactive Web-Services.

The main goal of these libraries is to remove the need to write boilerplate code. Using a declarative style, an engineer can define C++ classes and their serializable members, then send and receive them over the network with no manual serialization code.

---

## Libraries

### Application Layer

1. **[NisseBolt](NisseBolt.html)** | [Internal Documentation](internal/NisseBolt.html)
   A Bolt-style application framework for building Slack bots in C++. Provides high-level, declarative handler registration for Slack events, slash commands, interactive actions, shortcuts, views, and options -- running on the NisseServer async framework with automatic request signing verification.

2. **[Mug](Mug.html)** | [Internal Documentation](internal/Mug.html)
   A configurable HTTP server application that dynamically loads shared-library plugins at runtime. Define your REST endpoints in plugins and Mug handles the server lifecycle, configuration, and hot-reloading.

3. **[ThorsMug](ThorsMug.html)** | [Internal Documentation](internal/ThorsMug.html)
   The plugin interface for Mug. Implement `MugPlugin` to register HTTP route handlers that are dynamically loaded and unloaded by the Mug server.

4. **[ThorsSlack](ThorsSlack.html)** | [Internal Documentation](internal/ThorsSlack.html)
   Type-safe C++ API for the Slack platform. Supports the Slack REST API, webhooks, event callbacks, slash commands, and Block Kit -- all with automatic JSON serialization via ThorsSerializer.

### Server Framework

5. **[NisseServer](NisseServer.html)** | [Internal Documentation](internal/NisseServer.html)
   An event-driven, coroutine-based C++ server framework. Provides non-blocking async I/O over TCP and TLS sockets with a synchronous-looking API powered by `boost::coroutines2` and libEvent.

6. **[NisseHTTP](NisseHTTP.html)** | [Internal Documentation](internal/NisseHTTP.html)
   HTTP/1.x layer built on NisseServer. Provides request parsing, response generation, URL routing with path captures, chunked transfer encoding, and client-side HTTP utilities.

### Core Libraries

7. **[ThorsSocket](ThorsSocket.html)** | [Internal Documentation](internal/ThorsSocket.html)
   Unified C++ socket library for files, pipes, TCP sockets, and TLS sockets. Exposes all transport types through `std::iostream` with optional non-blocking I/O and coroutine yield support.

8. **[ThorsCrypto](ThorsCrypto.html)** | [Internal Documentation](internal/ThorsCrypto.html)
   Header-only cryptographic primitives: MD5, SHA-1, SHA-256 hashing, HMAC, PBKDF2 key derivation, SCRAM authentication, Base64 encoding/decoding, and CRC32 checksums.

9. **[ThorsSerializer](ThorsSerialize.html)** | [Internal Documentation](internal/ThorsSerialize.html)
   Declarative C++ serialization for JSON, YAML, and BSON. Declare which members to serialize with a single macro and use `operator<<` / `operator>>` for automatic conversion.

10. **[ThorsMongo](ThorsMongo.html)** | [Internal Documentation](internal/ThorsMongo.html)
    Type-safe MongoDB client for C++20. Builds BSON commands via ThorsSerializer, sends them over ThorsSocket, and exposes results as normal C++ objects and cursor-backed ranges.

11. **[ThorsLogging](ThorsLogging.html)** | [Internal Documentation](internal/ThorsLogging.html)
    Leveled logging macros with optional exception throwing. Wraps loguru for full builds or falls back to `std::cerr` in header-only mode.

---

## Installation

Requires a C++20 compatible compiler.

### Homebrew (Mac and Linux)

```bash
brew install thors-anvil
```

### Header-Only (from GitHub)

```bash
git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsAnvil.git
```

Add the cloned directory to your include path.

### From Source

```bash
git clone https://github.com/Loki-Astari/ThorsAnvil.git
cd ThorsAnvil
./configure
make
sudo make install
```

---

## Build Instructions

```bash
# Basic compilation
g++ -std=c++20 myfile.cpp -lThorSerialize -lThorsLogging

# With YAML support
g++ -std=c++20 myfile.cpp -lThorSerialize -lThorsLogging -lyaml

# Windows: add /Zc:preprocessor for conforming preprocessor
```
