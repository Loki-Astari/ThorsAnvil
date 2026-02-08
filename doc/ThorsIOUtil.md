# ThorsIOUtil Developer Documentation

## Overview

**ThorsIOUtil** is a C++ header-only library in the `ThorsAnvil::IOUtil` namespace that provides type-safe `printf`-style formatting via C++ `std::ostream`. It parses `printf` conversion specifiers at runtime, validates them against the actual C++ argument types, and serializes values using stream manipulators rather than the C `printf` family.

The library lives under `ThorsAnvil::IOUtil` (formatting) and `ThorsAnvil::Utility` (stream helpers and error utilities).

---

## File Map

| File | Purpose |
|---|---|
| `Format.h` | Top-level API. Defines the `make_format()` / `make_cppformat()` factory functions. |


## Free Functions

### Factory Functions (Format.h)

```cpp
template<typename... Args>
Format<Args...> make_format(char const* fmt, Args const&... args);
```
Creates a `Format` object with `Strictness::CTypeCompat`. Allows C-style implicit integer promotions.

```cpp
template<typename... Args>
Format<Args...> make_cppformat(char const* fmt, Args const&... args);
```
Creates a `Format` object with `Strictness::CPPTypeStrict`. Requires exact type matches.

## Formatting Pipeline

The end-to-end flow when you write:

```cpp
std::cout << make_format("Hello %s, you are %d years old.\n", name, age);
```

1. **`make_format()`** constructs a `Format<char const*, int>` with `Strictness::CTypeCompat`.
2. **`Format` constructor** parses the format string:
   - Extracts `prefixString[0] = "Hello "`, creates `Formatter` for `%s`.
   - Extracts `prefixString[1] = ", you are "`, creates `Formatter` for `%d`.
   - Extracts `prefixString[2] = " years old.\n"`.
   - Validates specifier count matches argument count.
3. **`operator<<(cout, format)`** calls `format.print(cout)`.
4. **`print()`** calls `doPrint()` which iterates over each index:
   - Prints `prefixString[I]` literally.
   - Streams `formater[I]` into the ostream, producing a `FormatterCheck`.
   - Streams `std::get<I>(arguments)` into the `FormatterCheck`, which calls `Formatter::apply()`.
5. **`apply()`** validates the argument type, then calls `applyData()`.
6. **`applyData()`** configures the stream (flags, fill, width, precision), calls `printToStream()`, then restores the stream state.
7. **`printToStream()`** dispatches to the appropriate printer (integer, string, or generic `operator<<`).
8. The trailing `prefixString.back()` is printed.

---

## Error Handling

All errors are thrown as `std::invalid_argument`:

- **Too few specifiers:** more arguments than `%`-specifiers.
- **Too many specifiers:** more `%`-specifiers than arguments.
- **Invalid specifier character:** unrecognised character after length modifier.
- **Invalid length + specifier combination:** e.g. `%hhf`.
- **Type mismatch:** argument type does not match the expected type for the specifier (and no valid conversion exists under the current `Strictness` mode).
- **Dynamic width/precision not `int`:** `*` arguments must be `int`.

---

## Usage Example

```cpp
#include "Format.h"
#include <iostream>

using namespace ThorsAnvil::IOUtil;

int main()
{
    // C-compatible mode (allows promotions)
    std::cout << make_format("Integer: %5.3d\n", 5);          // "Integer:   005"
    std::cout << make_format("%s is %d\n", "answer", 42);     // "answer is 42"

    // Strict C++ mode
    std::cout << make_cppformat("Value: %d\n", 10);           // OK: int matches %d
    // std::cout << make_cppformat("Value: %d\n", 10LL);      // THROWS: long long != int

    // Dynamic width
    std::cout << make_format("%*d\n", 10, 42);                // "        42"
}
```
