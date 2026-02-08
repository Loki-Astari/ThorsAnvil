# ThorsLogging Developer Documentation

## Overview

**ThorsLogging** (namespace `ThorsAnvil::Logging`) is a thin C++ wrapper around the [loguru](https://github.com/emilk/loguru) logging library. It provides a set of preprocessor macros and a utility class for structured, leveled logging with optional exception throwing. The library supports both a full-featured linked mode (using loguru) and a lightweight header-only mode that falls back to `std::cerr`.

### Dependencies

- **loguru** -- bundled as a git submodule under `third/loguru/`
- **ThorsIOUtil** -- provides `ThorsAnvil::Utility::buildErrorMessage()`, used to format all log messages

## Architecture

```
ThorsLogging.h                  <-- Public API (macros + ThorsLogTemp class)
```

The entire public surface is in a single header: `ThorsLogging.h`. There are no additional `.cpp` files in this library; the implementation is entirely macro- and inline-based.

---

## Build Modes

ThorsLogging supports two build modes controlled by the preprocessor symbol `THORS_LOGGING_HEADER_ONLY`:

| Mode | `THORS_LOGGING_HEADER_ONLY` | Backend | Notes |
|------|---------------------------|---------|-------|
| **Linked** | `0` or undefined | loguru | Full loguru functionality; requires linking against the loguru library. |
| **Header-only** | `1` | `std::cerr` | No link dependency on loguru. Provides a minimal `loguru::NamedVerbosity` enum and routes output to `std::cerr`. |

In header-only mode, the compile-time constant `THOR_LOGGING_DEFAULT_LOG_LEVEL` (default: `3`) determines which messages are emitted. Messages with a verbosity numerically greater than this threshold are suppressed at compile time.

---

## Verbosity Levels

ThorsLogging inherits loguru's verbosity model. Lower numeric values are more severe:

| Named Level | Numeric Value | Macro Suffix | Description |
|-------------|---------------|--------------|-------------|
| `FATAL`     | -3            | `Fatal`      | Unrecoverable error; application will terminate. |
| `ERROR`     | -2            | `Error`      | Serious error requiring attention. |
| `WARNING`   | -1            | `Warning`    | Potential problem or unexpected condition. |
| `INFO`      |  0            | `Info`       | Normal operational messages (default stderr threshold). |
| (debug)     |  3            | `Debug`      | Detailed diagnostic information. |
| (track)     |  5            | `Track`      | Coarse-grained tracing. |
| (trace)     |  7            | `Trace`      | Fine-grained tracing. |
| (all)       |  9            | `All`        | Maximum verbosity. |

Special values:

- `Verbosity_OFF` (-9) -- Can be assigned to `g_stderr_verbosity` to suppress all output.
- `Verbosity_INVALID` (-10) -- Sentinel; never log at this level.

---

## Public API Reference

### Log-Only Macros

These macros log a message at the specified verbosity level. They do **not** throw exceptions.

```cpp
ThorsLogFatal(Scope, Function, ...)
ThorsLogError(Scope, Function, ...)
ThorsLogWarning(Scope, Function, ...)
ThorsLogInfo(Scope, Function, ...)
ThorsLogDebug(Scope, Function, ...)
ThorsLogTrack(Scope, Function, ...)
ThorsLogTrace(Scope, Function, ...)
ThorsLogAll(Scope, Function, ...)
```

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `Scope`   | String literal / identifier | Typically the class name or module name. |
| `Function`| String literal / identifier | The function or method name. |
| `...`     | Variadic (streamable) | One or more arguments that are concatenated via `ThorsAnvil::Utility::buildErrorMessage()`. Any type that supports `operator<<` to a `std::ostream` is accepted. |

**Example:**

```cpp
void MyClass::doWork(int id)
{
    ThorsLogInfo("MyClass", "doWork", "Processing item: ", id);
    // ...
    ThorsLogDebug("MyClass", "doWork", "Finished processing item: ", id);
}
```

**Behavior:**

- In linked mode: delegates to loguru's `VLOG_S` / `LOG_F` macros.
- In header-only mode: writes to `std::cerr` if the level <= `THOR_LOGGING_DEFAULT_LOG_LEVEL`.
- `ThorsLogFatal` will cause the application to abort (via loguru's `FATAL` handling in linked mode, or via `std::cerr` output in header-only mode).

---

### Log-and-Throw Macros

These macros log a message **and** throw an exception of the specified type with the formatted message.

```cpp
ThorsLogAndThrowFatal(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowError(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowWarning(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowInfo(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowDebug(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowTrack(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowTrace(ExceptionType, Scope, Function, ...)
ThorsLogAndThrowAll(ExceptionType, Scope, Function, ...)
```

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `ExceptionType` | Type name | Any exception class constructible from `std::string`. |
| `Scope`   | String literal / identifier | Class or module name. |
| `Function`| String literal / identifier | Function or method name. |
| `...`     | Variadic (streamable) | Message components. |

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

**Behavior:**

The message is first logged at the specified level via `ThorsMessage`, then the same message string is used to construct and throw the specified exception. The throw is controlled via `if constexpr`, so the compiler can optimize away the exception path when using the log-only variants.

---

### Stream-Style Logging

```cpp
ThorsMessage(Level, ...)
```

A lower-level macro that logs a stream-formatted message using `VLOG_S`. Used internally by the other macros but also available for direct use. `Level` is the raw loguru level suffix (e.g., `INFO`, `WARNING`, `3`, `5`).

---

### Exception Catch/Rethrow Helpers

```cpp
ThorsCatchMessage(Scope, Function, exceptionObject)
ThorsRethrowMessage(Scope, Function, exceptionObject)
```

Convenience macros for logging caught exceptions at verbosity level 2. Use `ThorsCatchMessage` when catching and handling, `ThorsRethrowMessage` when catching and rethrowing.

**Example:**

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

### Verbosity Control Macros

```cpp
ThorsLogLevel(level)        // Set verbosity: loguru::g_stderr_verbosity = Verbosity_<level>
ThorsLogLevelSet(level)     // Set verbosity to a raw int value
ThorsLogLevelGet()          // Returns current verbosity (int)
ThorsLogLevelItem(level)    // Expands to loguru::Verbosity_<level>
```

**Example:**

```cpp
ThorsLogLevel(WARNING);              // Only FATAL, ERROR, WARNING will print
int prev = ThorsLogLevelGet();       // Save current level
ThorsLogLevelSet(9);                 // Enable all messages
// ... verbose section ...
ThorsLogLevelSet(prev);             // Restore
```

---

### ThorsLogTemp Class

An RAII guard that temporarily changes the stderr verbosity for its lifetime and restores the previous level on destruction.

```cpp
class ThorsLogTemp
{
public:
    ThorsLogTemp(loguru::NamedVerbosity level);
    ThorsLogTemp(int level);
    ~ThorsLogTemp();  // restores previous verbosity
};
```

**Example:**

```cpp
void MyClass::debugSection()
{
    ThorsLogTemp guard(loguru::Verbosity_9);  // enable all logging
    // ... everything in this scope logs at max verbosity ...
}   // previous verbosity restored here
```

In header-only mode, `ThorsLogTemp` is a no-op (constructor/destructor do nothing).

---

## Internal Macro Machinery

The public macros are built on two internal macros:

### `ThorsLogActionWithPotetialThrow(hasExcept, Exception, Level, Scope, Function, ...)`

Core implementation macro. When `hasExcept` is `true`, it logs and throws. When `false`, it only logs. The throw path uses `if constexpr` so it is compiled away when not needed.

### `ThorsLogAction(...)` / `ThorsLogAndThrowAction(...)`

Thin wrappers:

- `ThorsLogAction(...)` -- calls `ThorsLogActionWithPotetialThrow` with `hasExcept=false` and `Exception=std::runtime_error` (the exception type is unused).
- `ThorsLogAndThrowAction(...)` -- calls `ThorsLogActionWithPotetialThrow` with `hasExcept=true`.

### Header-Only Internals

When `THORS_LOGGING_HEADER_ONLY == 1`:

- **`ConvertToVoid`** -- A helper class whose `operator&(std::ostream&)` discards the stream expression. Used by `VLOG_IF_S` to conditionally suppress output at compile time.
- **`VLOG_IF_S(verbosity, cond)`** -- Expands to either `(void)0` (suppressed) or `std::cerr` (active), based on verbosity and condition.
- **`VLOG_S(verbosity)`** -- Shorthand for `VLOG_IF_S(verbosity, true)`.
- **`ThorsLogOutput(Level, message)`** -- Writes to `std::cerr` if the level is within threshold.

---

## Configuration Reference

### Preprocessor Symbols

| Symbol | Default | Description |
|--------|---------|-------------|
| `THORS_LOGGING_HEADER_ONLY` | undefined (0) | Set to `1` for header-only mode. Generated into `ThorsLoggingConfig.h`. |
| `THORS_LOGGING_HEADER_ONLY_INCLUDE` | undefined | Controls inline qualifier for header-only builds. |
| `THOR_LOGGING_DEFAULT_LOG_LEVEL` | `3` | Header-only mode only. Compile-time verbosity ceiling. |
| `LOGURU_WITH_STREAMS` | `1` | Enables loguru's stream-based logging API (`LOG_S`, `VLOG_S`). Always set by ThorsLogging. |

### Build System

The project uses Autotools:

- `configure.ac` defines the project and checks for `libdl`.
- `AX_THOR_FEATURE_HEADER_ONLY_VARIANT(THORS_LOGGING)` provides the `--enable-header-only` configure flag.
- `ThorsLoggingConfig.h` is generated from `config.h.in` during configuration.
- The `Makefile` adds `-DLOGURU_WITH_STREAMS=1` to `CXXFLAGS`.

---

## Usage Patterns

### Basic Logging in a Class

```cpp
#include "ThorsLogging/ThorsLogging.h"

class NetworkClient
{
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
```

### Temporarily Increasing Verbosity

```cpp
void diagnose()
{
    ThorsLogTemp verbose(9);  // max verbosity for this scope
    ThorsLogAll("Diagnostics", "diagnose", "Starting full diagnostic...");
    // ... detailed logging throughout ...
}
```

### Catch and Log Pattern

```cpp
try {
    processRequest(req);
}
catch (std::exception const& e) {
    ThorsCatchMessage("Server", "handleRequest", e.what());
    sendErrorResponse(500);
}
```

### Setting Global Verbosity at Startup

```cpp
int main(int argc, char* argv[])
{
    // Only show warnings and above by default
    ThorsLogLevel(WARNING);

    if (argc > 1 && std::string(argv[1]) == "--verbose") {
        ThorsLogLevel(INFO);
    }
    // ...
}
```

---

## Extending ThorsLogging

### Custom Exception Types

Any exception type constructible from `std::string` works with the log-and-throw macros:

```cpp
class AppError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

ThorsLogAndThrowError(AppError, "Module", "func", "something broke");
```

### Integration with ThorsSerializer

The `ThorsLogXXXWithData` macros (documented in the README but defined externally in ThorsSerializer) append a JSON-serialized object to the log message. These follow the same pattern as the base macros but accept an additional `data` parameter as the first argument.
