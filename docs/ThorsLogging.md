[Home](index.html) | [Internal Documentation](internal/ThorsLogging.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsLogging API

Leveled logging macros for C++ with optional exception throwing. Wraps [loguru](https://github.com/emilk/loguru) in full builds or falls back to `std::cerr` in header-only mode.

**Namespace:** `ThorsAnvil::Logging`

---

## Header

```cpp
#include "ThorsLogging/ThorsLogging.h"
```

---

## Log-Only Macros

Log a message at a specified verbosity level. These do **not** throw exceptions.

```cpp
ThorsLogFatal(Scope, Function, ...)
ThorsLogError(Scope, Function, ...)
ThorsLogWarning(Scope, Function, ...)
ThorsLogInfo(Scope, Function, ...)
ThorsLogDebug(Scope, Function, ...)
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `Scope` | Class or module name (string literal) |
| `Function` | Function name (string literal) |
| `...` | One or more streamable values, concatenated into the message |

**Example:**

```cpp
void MyClass::doWork(int id)
{
    ThorsLogInfo("MyClass", "doWork", "Processing item: ", id);
    ThorsLogDebug("MyClass", "doWork", "Finished processing item: ", id);
}
```

---

## Log-and-Throw Macros

Log a message **and** throw an exception of the specified type:

```cpp
ThorsLogAndThrowFatal(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowError(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowWarning(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowInfo(ExceptionType, Scope, Function, ...)
```

**Example:**

```cpp
void MyClass::open(std::string const& path)
{
    if (!fileExists(path)) {
        ThorsLogAndThrowError(std::runtime_error, "MyClass", "open",
                              "File not found: ", path);
    }
}
```

The exception is constructed from the formatted message string. Any exception type constructible from `std::string` works.

---

## Verbosity Levels

| Level | Numeric | Description |
|-------|---------|-------------|
| `FATAL` | -3 | Unrecoverable error; application terminates |
| `ERROR` | -2 | Serious error requiring attention |
| `WARNING` | -1 | Potential problem |
| `INFO` | 0 | Normal operational messages |
| (debug) | 3 | Detailed diagnostics |

---

## Verbosity Control

```cpp
// Set verbosity level
ThorsLogLevel(WARNING);           // Only FATAL, ERROR, WARNING will print

// Save and restore
int prev = ThorsLogLevelGet();
ThorsLogLevelSet(9);              // Enable all messages
// ... verbose section ...
ThorsLogLevelSet(prev);           // Restore
```

---

## Temporary Verbosity (RAII)

`ThorsLogTemp` temporarily changes the verbosity for the duration of a scope:

```cpp
void diagnose()
{
    ThorsLogTemp verbose(9);  // max verbosity
    ThorsLogAll("Diagnostics", "diagnose", "Starting full diagnostic...");
    // ... detailed logging throughout ...
}   // previous verbosity restored here
```

---

## Catch/Rethrow Helpers

```cpp
try {
    riskyOperation();
}
catch (std::exception const& e) {
    ThorsCatchMessage("MyClass", "run", e.what());
    // handle...
}
```

---

## Build Modes

| Mode | Define | Backend |
|------|--------|---------|
| Linked | `THORS_LOGGING_HEADER_ONLY=0` (default) | loguru (full features) |
| Header-only | `THORS_LOGGING_HEADER_ONLY=1` | `std::cerr` (no link dependency) |

In header-only mode, `THOR_LOGGING_DEFAULT_LOG_LEVEL` (default: `3`) controls the compile-time verbosity ceiling.

---

## Complete Example

### Makefile
```Makefile
CXXFLAGS    = -std=c++20

LDLIBS      = -lThorsLogging

all:        ThorsLogApp
```
### ThorsLogApp.cpp

```cpp
#include "ThorsLogging/ThorsLogging.h"
#include <stdexcept>

class NetworkClient
{
    // Example fail
    bool doConnect(std::string const& host, int port) {
        return false;
    }
public:
    void connect(std::string const& host, int port)
    {
        ThorsLogInfo("NetworkClient", "connect",
                     "Connecting to ", host, ":", port);

        if (!doConnect(host, port)) {
            ThorsLogAndThrowError(std::runtime_error,
                                  "NetworkClient", "connect",
                                  "Failed to connect to ", host, ":", port);
        }

        ThorsLogDebug("NetworkClient", "connect", "Connection established");
    }
};

int main()
{
    ThorsLogLevel(INFO);  // show INFO and above

    NetworkClient client;
    try {
        client.connect("example.com", 8080);
    }
    catch (std::exception const& e) {
        ThorsCatchMessage("main", "main", e.what());
    }
}
```
