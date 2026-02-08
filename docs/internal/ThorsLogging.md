# ThorsLogging Internal Documentation

Implementation details, macro machinery, and build-mode internals for the ThorsLogging library.

**Source:** `third/ThorsLogging/src/ThorsLogging/`

---

## Architecture

The entire public surface is in a single header: `ThorsLogging.h`. There are no additional `.cpp` files; the implementation is entirely macro- and inline-based.

```
ThorsLogging.h          <-- Public API (macros + ThorsLogTemp class)
ThorsLoggingConfig.h    <-- Generated build configuration
```

### Dependencies

- **loguru** -- bundled as a git submodule under `third/loguru/`
- **ThorsIOUtil** -- provides `ThorsAnvil::Utility::buildErrorMessage()` for formatting all log messages

---

## Build Modes

Controlled by the preprocessor symbol `THORS_LOGGING_HEADER_ONLY`:

| Mode | `THORS_LOGGING_HEADER_ONLY` | Backend | Notes |
|------|---------------------------|---------|-------|
| **Linked** | `0` or undefined | loguru | Full loguru functionality; requires linking against loguru. |
| **Header-only** | `1` | `std::cerr` | No link dependency. Minimal `loguru::NamedVerbosity` enum provided. |

In header-only mode, `THOR_LOGGING_DEFAULT_LOG_LEVEL` (default: `3`) determines which messages are emitted. Messages with verbosity numerically greater than this threshold are suppressed at compile time.

---

## Verbosity Model

Inherits loguru's verbosity model. Lower numeric values are more severe:

| Named Level | Numeric | Macro Suffix |
|-------------|---------|--------------|
| `FATAL` | -3 | `Fatal` |
| `ERROR` | -2 | `Error` |
| `WARNING` | -1 | `Warning` |
| `INFO` | 0 | `Info` |
| (debug) | 3 | `Debug` |
| (track) | 5 | `Track` |
| (trace) | 7 | `Trace` |
| (all) | 9 | `All` |

Special values:
- `Verbosity_OFF` (-9) -- suppress all output
- `Verbosity_INVALID` (-10) -- sentinel; never log at this level

---

## Internal Macro Machinery

### Core Implementation Macro

```cpp
ThorsLogActionWithPotetialThrow(hasExcept, Exception, Level, Scope, Function, ...)
```

When `hasExcept` is `true`, it logs and throws. When `false`, it only logs. The throw path uses `if constexpr` so it is compiled away when not needed.

### Wrapper Macros

- `ThorsLogAction(...)` -- calls core macro with `hasExcept=false` and `Exception=std::runtime_error` (unused).
- `ThorsLogAndThrowAction(...)` -- calls core macro with `hasExcept=true`.

### Message Formatting

All messages are formatted via `ThorsAnvil::Utility::buildErrorMessage(...)` which concatenates all variadic arguments using `operator<<` to a `std::ostringstream`.

---

## Header-Only Mode Internals

When `THORS_LOGGING_HEADER_ONLY == 1`:

### ConvertToVoid

A helper class whose `operator&(std::ostream&)` discards the stream expression. Used by `VLOG_IF_S` to conditionally suppress output at compile time.

### VLOG_IF_S(verbosity, cond)

Expands to either `(void)0` (suppressed) or `std::cerr` (active), based on verbosity and condition.

### VLOG_S(verbosity)

Shorthand for `VLOG_IF_S(verbosity, true)`.

### ThorsLogOutput(Level, message)

Writes to `std::cerr` if the level is within threshold.

### ThorsLogTemp (Header-Only)

In header-only mode, `ThorsLogTemp` is a no-op (constructor/destructor do nothing).

---

## Linked Mode Internals

In linked mode, the macros delegate to loguru's `VLOG_S` / `LOG_F` macros, which provide:
- Colored console output
- File logging
- Syslog support
- Stack traces on fatal errors
- Thread-safe output

`ThorsLogFatal` causes the application to abort via loguru's `FATAL` handling.

---

## Configuration Reference

### Preprocessor Symbols

| Symbol | Default | Description |
|--------|---------|-------------|
| `THORS_LOGGING_HEADER_ONLY` | 0 | Set to `1` for header-only mode |
| `THOR_LOGGING_DEFAULT_LOG_LEVEL` | `3` | Header-only compile-time verbosity ceiling |
| `LOGURU_WITH_STREAMS` | `1` | Enables loguru's stream-based API. Always set. |

### Build System

- `configure.ac` defines the project and checks for `libdl`.
- `AX_THOR_FEATURE_HEADER_ONLY_VARIANT(THORS_LOGGING)` provides `--enable-header-only`.
- `ThorsLoggingConfig.h` is generated from `config.h.in`.
- The `Makefile` adds `-DLOGURU_WITH_STREAMS=1` to `CXXFLAGS`.
