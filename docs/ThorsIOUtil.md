# ThorsIOUtil API

Type-safe `printf`-style formatting through `std::ostream`. Validates format specifiers against C++ argument types at runtime and serializes values using stream manipulators.

**Namespace:** `ThorsAnvil::IOUtil`

---

## Header

```cpp
#include "ThorsIOUtil/Format.h"
```

---

## Factory Functions

### make_format

```cpp
template<typename... Args>
Format<Args...> make_format(char const* fmt, Args const&... args);
```

Creates a format object with **C-compatible** type checking. Allows implicit integer promotions (e.g., `short` matches `%d`).

### make_cppformat

```cpp
template<typename... Args>
Format<Args...> make_cppformat(char const* fmt, Args const&... args);
```

Creates a format object with **strict C++** type checking. Requires exact type matches.

---

## Usage

Stream the format object to any `std::ostream`:

```cpp
#include "ThorsIOUtil/Format.h"
#include <iostream>

using namespace ThorsAnvil::IOUtil;

int main()
{
    // C-compatible mode (allows promotions)
    std::cout << make_format("Hello %s, you are %d years old.\n", "Alice", 30);

    // Formatted integers
    std::cout << make_format("Integer: %5.3d\n", 5);     // "Integer:   005"

    // Strict C++ mode
    std::cout << make_cppformat("Value: %d\n", 10);      // OK: int matches %d
    // std::cout << make_cppformat("Value: %d\n", 10LL); // THROWS: long long != int

    // Dynamic width
    std::cout << make_format("%*d\n", 10, 42);            // "        42"
}
```

---

## Supported Specifiers

All standard `printf` conversion specifiers are supported: `%d`, `%i`, `%u`, `%x`, `%X`, `%o`, `%f`, `%e`, `%g`, `%s`, `%c`, `%p`, `%%`, etc.

Length modifiers (`h`, `hh`, `l`, `ll`, `z`, `j`, `t`, `L`) are validated against argument types.

Dynamic width (`*`) and precision (`.*`) are supported -- the corresponding arguments must be `int`.

---

## Error Handling

All errors throw `std::invalid_argument`:

- Too few or too many format specifiers for the given arguments
- Invalid specifier character or length/specifier combination
- Type mismatch between argument and specifier
- Dynamic width/precision argument is not `int`
