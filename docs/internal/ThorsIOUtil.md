[Home](../index.html) | [API Documentation](../ThorsIOUtil.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsIOUtil Internal Documentation

Implementation details for the ThorsIOUtil formatting and utility library.

**Source:** `third/ThorsIOUtil/src/ThorsIOUtil/`

---

## Table of Contents

- [File Map](#file-map)
- [Enumerations](#enumerations)
- [FormatInfo Struct](#formatinfo-struct)
- [Format\<Args...\> Class](#formatargs-class)
- [Formatter Class](#formatter-class)
- [FormatterCheck Class](#formattercheck-class)
- [NormalizeChar\<T\> Trait](#normalizechart-trait)
- [SignConversionOption\<T\> Trait](#signconversionoptiont-trait)
- [Print Dispatch Functions](#print-dispatch-functions)
- [Dynamic Width/Precision Storage](#dynamic-widthprecision-storage)
- [Utility Functions](#utility-functions)
- [StreamFormatterNoChange Class](#streamformatternochange-class)
- [Formatting Pipeline](#formatting-pipeline)
- [Type Resolution Table](#type-resolution-table)
- [Error Handling](#error-handling)

---

## File Map

| File | Namespace | Purpose |
|------|-----------|---------|
| `Format.h` | `ThorsAnvil::IOUtil` | `Format<Args...>` class, `make_format()`, `make_cppformat()` factory functions |
| `FormatInfo.h` | `ThorsAnvil::IOUtil` | Enums (`Strictness`, `Length`, `Specifier`, `Type`, `Dynamic`) and `FormatInfo` struct |
| `Formatter.h` | `ThorsAnvil::IOUtil` | `Formatter` class (parses one `%`-specifier), `FormatterCheck`, `NormalizeChar`, type resolution map |
| `SignConversionOption.h` | `ThorsAnvil::IOUtil` | `SignConversionOption<T>` trait for signed/unsigned integer conversions |
| `printToStream.h` | `ThorsAnvil::IOUtil` | Top-level `printToStream()` dispatch (generic, integer, string) |
| `printIntToStream.h` | `ThorsAnvil::IOUtil` | `printIntToStream()` -- integer formatting with precision/width handling |
| `printStringToStream.h` | `ThorsAnvil::IOUtil` | `printStringToStream()` -- string formatting with precision truncation |
| `saveToStream.h` | `ThorsAnvil::IOUtil` | `saveToStream()` -- stores dynamic width/precision in `std::ostream::iword()` |
| `Utility.h` | `ThorsAnvil::Utility` | `StreamFormatterNoChange`, `buildStringFromParts()`, `buildErrorMessage()`, `systemErrorMessage()` |

---

## Enumerations

Defined in `FormatInfo.h`:

### Strictness

```cpp
enum class Strictness { CPPTypeStrict, CTypeCompat };
```

| Value | Description |
|-------|-------------|
| `CTypeCompat` | Allows C-style implicit integer promotions. `char` and `short` are accepted for `%d`. An `int` argument can be truncated to match `%hhd` or `%hd`. |
| `CPPTypeStrict` | Requires exact type matches. `short` does not match `%d` (expects `int`). |

### Length

```cpp
enum class Length { none, hh, h, l, ll, j, z, t, L };
```

Maps directly to C `printf` length modifiers.

### Specifier

```cpp
enum class Specifier { d, i, u, o, x, X, f, F, e, E, g, G, a, A, c, s, p, n };
```

Maps directly to C `printf` conversion specifiers.

### Type

```cpp
enum class Type { Int, UInt, Float, Char, String, Pointer, Count };
```

The logical type category derived from the `Specifier`. Used to select the printing strategy.

### Dynamic

```cpp
enum class Dynamic { None, Width, Precision, Both };
```

Tracks whether width and/or precision are specified via `*` (dynamic) arguments rather than literal values in the format string.

---

## FormatInfo Struct

Defined in `FormatInfo.h`. Holds all parsed data from a single `%`-specifier:

```cpp
struct FormatInfo
{
    // Flags (from characters after '%')
    bool                    leftJustify;     // '-' flag
    bool                    forceSign;       // '+' flag
    bool                    forceSignWidth;  // ' ' (space) flag
    bool                    prefixType;      // '#' flag
    bool                    leftPad;         // '0' flag

    // Dimensions
    std::size_t             width;
    std::size_t             precision;       // -1 (as size_t) means "not specified"

    // Parsed components
    Length                  length;
    Specifier               specifier;

    // Pre-computed values
    Type                    type;            // Derived from length + specifier
    Dynamic                 useDynamicSize;  // Whether width/precision come from arguments
    AllowedType             expectedType;    // {type_info*, truncation_mask}
    std::ios_base::fmtflags format;          // Pre-built stream flags
};
```

`AllowedType` is `std::pair<std::type_info const*, int>` where the second element is a bitmask used for integer truncation (e.g., `0xFF` for `%hhd`, `0xFFFF` for `%hd`, `0` for no truncation).

---

## Format\<Args...\> Class

Defined in `Format.h`. **Namespace:** `ThorsAnvil::IOUtil`.

The top-level object returned by `make_format()` and `make_cppformat()`. It holds const references to all arguments and the parsed format string data.

### Members

| Member | Type | Description |
|--------|------|-------------|
| `strict` | `Strictness` | Type-checking mode |
| `format` | `std::string` | The original format string |
| `arguments` | `std::tuple<Args const&...>` | Const references to all arguments |
| `prefixString` | `std::vector<std::string>` | Literal text segments between specifiers. Size = N+1 for N arguments. |
| `formater` | `std::vector<Formatter>` | One `Formatter` per `%`-specifier (including dynamic width/precision pseudo-formatters). |

### Constructor

```cpp
Format(Strictness strict, char const* fmt, Args const&... args);
```

Parses the format string in a single pass:

1. For each argument, calls `getNextPrefix()` to extract the literal text up to the next `%`.
2. Creates a `Formatter` from the `%`-specifier at the current position.
3. Tracks dynamic width/precision state: if a `Formatter` reports `isDynamicSize() == Dynamic::Width`, the next argument is consumed as the width value (not a regular argument). Same for `Dynamic::Precision`. If both width and precision are dynamic, two consecutive arguments are consumed.
4. After all arguments, calls `getNextPrefix()` once more for the trailing literal text. Throws if another `%`-specifier is found (too many specifiers).

### Methods

| Method | Description |
|--------|-------------|
| `print(std::ostream& s) const` | Serializes the formatted output to stream `s`. |
| `operator<<(std::ostream&, Format const&)` | Calls `print()`. Returns the stream. |

### Private Methods

| Method | Description |
|--------|-------------|
| `getNextPrefix(format, pos, test, mes)` | Scans from `pos` for the next unescaped `%`. Collapses `%%` into literal `%`. Returns `{prefix_text, chars_consumed}`. Throws `std::invalid_argument` if the `test` predicate fails. |
| `printValue<I>(std::ostream& s)` | Prints `prefixString[I]`, then streams `formater[I]` and `std::get<I>(arguments)` through the `FormatterCheck` mechanism. |
| `doPrint(std::ostream& s, std::index_sequence<I...>)` | Expands `printValue<I>` for each index in the pack, then prints `prefixString.back()`. Uses a dummy array expansion to call each `printValue` in order. |

---

## Formatter Class

Defined in `Formatter.h`. **Namespace:** `ThorsAnvil::IOUtil`.

Parses a single `%`-specifier from the format string and provides the logic to apply it to an argument.

### Members

| Member | Type | Description |
|--------|------|-------------|
| `strict` | `Strictness` | Type-checking mode inherited from `Format` |
| `used` | `std::size_t` | Number of characters consumed from the format string |
| `dynamicSize` | `Dynamic` | If this formatter is a pseudo-formatter for a dynamic `*` width or precision |
| `info` | `FormatInfo` | All parsed specifier data |

### Constructor

```cpp
Formatter(Strictness strict, char const* formatStr, Dynamic dynamicWidthHandeled);
```

Parses the format string starting at the `%` character. The parsing sequence is:

1. **Flags** (`-`, `+`, ` `, `#`, `0`): Loop until no more flags found.
2. **Width**: Digits or `*`. If `*` and not yet handled, sets `dynamicSize = Dynamic::Width` and returns immediately (this formatter becomes a width-consumer, not a real specifier).
3. **Precision**: `.` followed by digits or `*`. Same dynamic handling as width.
4. **Length modifier**: `h`, `hh`, `l`, `ll`, `j`, `z`, `t`, `L`.
5. **Specifier**: `d`, `i`, `u`, `o`, `x`, `X`, `f`, `F`, `e`, `E`, `g`, `G`, `a`, `A`, `c`, `s`, `p`, `n`. Throws on unrecognized character.
6. **Post-processing**: Calls `getType()` to resolve the expected C++ type. Pre-computes `std::ios_base::fmtflags` for the stream.

### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `size()` | `std::size_t` | Characters consumed from the format string (0 if this is a dynamic pseudo-formatter) |
| `isDynamicSize()` | `Dynamic` | Whether this is a dynamic width/precision consumer |
| `operator<<(ostream&, Formatter const&)` | `FormatterCheck` | Creates a `FormatterCheck` that will call `apply()` when the argument is streamed |

### Private Methods

| Method | Description |
|--------|-------------|
| `apply(std::ostream& s, A const& argInput)` | Type-checks the argument against `info.expectedType`, then either calls `applyData()` (normal specifier) or `saveToStream()` (dynamic width/precision). See [Type Checking in apply()](#type-checking-in-apply) below. |
| `applyData(std::ostream& s, A const& arg)` | Saves stream state, configures stream flags/fill/width/precision, calls `printToStream()`, restores stream state. Handles dynamic width/precision by reading from `s.iword()`. |
| `getType(Length, Type)` | Static. Returns `AllowedType` from a lookup map of all valid `(Type, Length)` combinations. Throws `std::invalid_argument` for invalid combinations. |

### Type Checking in apply()

The `apply()` method performs a multi-step type resolution:

1. Normalizes `signed char` to `char` via `NormalizeChar<A>`.
2. Gets `Actual` and `Alternative` types from `SignConversionOption<A>`.
3. **Exact match**: If `typeid(Actual) == expectedType`, calls `applyData()` directly.
4. **Signed/unsigned alternative**: If `typeid(Alternative) == expectedType`, casts and calls `applyData()`. This allows e.g. `unsigned int` to match `%d` (which expects `int`).
5. **C promotion** (CTypeCompat only): If `SignConversionOption<A>::allowIntConversion` is true, converts to `int` via `convertToInt()`. This handles `char` and `short` matching `%d`.
6. **Integer truncation** (CTypeCompat only): If the argument is `int` and `expectedType` has a non-zero truncation mask, truncates via bitwise AND. This handles `int` matching `%hhd` or `%hd`.
7. **Failure**: Throws `std::invalid_argument` with the expected and actual type names.

For **dynamic width/precision** (`dynamicSize != Dynamic::None`): verifies the argument is `int`, then calls `saveToStream()` to store the value in the stream's `iword()` slot.

---

## FormatterCheck Class

Defined as a nested struct inside `Formatter`. Acts as a temporary bridge between streaming the `Formatter` and streaming the argument.

```cpp
struct FormatterCheck
{
    std::ostream&       stream;
    Formatter const&    formatter;

    template<typename A>
    std::ostream& operator<<(A const& nextArg);
};
```

The expression `stream << formatter << argument` works as:
1. `stream << formatter` returns a `FormatterCheck` (via `Formatter::operator<<`).
2. `FormatterCheck << argument` calls `formatter.apply(stream, argument)` and returns `stream`.

---

## NormalizeChar\<T\> Trait

Defined in `printToStream.h`. Normalizes `signed char` to `char` so they are treated as the same type during type checking:

```cpp
template<typename T> struct NormalizeChar       { using Type = T; };
template<>           struct NormalizeChar<signed char> { using Type = char; };
```

---

## SignConversionOption\<T\> Trait

Defined in `SignConversionOption.h`. Controls signed/unsigned conversions and C-style promotions for each integer type.

### Interface

```cpp
template<typename T>
struct SignConversionOption
{
    using Actual       = T;          // The actual type
    using Alternative  = T;          // Acceptable alternative (signed <-> unsigned counterpart)
    static constexpr bool allowIntConversion = false;  // Can promote to int?
    static int convertToInt(T const&);    // Promotion to int
    static int truncate(T const&, int);   // Truncation via bitmask (int only)
};
```

### Specializations

| Type | Alternative | allowIntConversion | Notes |
|------|-------------|--------------------|-------|
| `char` | `unsigned char` | `true` | Promotes to `int` via cast |
| `unsigned char` | `char` | `true` | Promotes to `int` via cast |
| `short` | `unsigned short` | `true` | Promotes to `int` via cast |
| `unsigned short` | `short` | `true` | Promotes to `int` via cast |
| `int` | `unsigned int` | `false` | Supports `truncate(arg, mask)` via `arg & mask` |
| `unsigned int` | `int` | `false` | -- |
| `long` | `unsigned long` | `false` | -- |
| `unsigned long` | `long` | `false` | -- |
| `long long` | `unsigned long long` | `false` | -- |
| `unsigned long long` | `long long` | `false` | -- |
| (default) | same as T | `false` | No conversions for non-integer types |

---

## Print Dispatch Functions

### printToStream() (Generic)

Defined in `printToStream.h`. Three overloads selected by SFINAE and overload resolution:

**1. Generic (non-integral types)**

```cpp
template<typename T>
enable_if<!is_integral<T>::value>::type
printToStream(std::ostream& s, T const& arg, FormatInfo const&);
```

Simply calls `s << arg`. Used for floating-point types and any type with `operator<<`.

**2. Integer types**

```cpp
template<typename T>
enable_if<is_integral<T>::value>::type
printToStream(std::ostream& s, T const& arg, FormatInfo const& info);
```

If `info.type == Type::Char`, outputs `static_cast<char>(arg)`. Otherwise, converts through `CharIntConverter<T>` (which maps `char` to `int` and `unsigned char` to `unsigned int`) and delegates to `printIntToStream()`.

**3. C strings**

```cpp
void printToStream(std::ostream& s, char const* const& arg, FormatInfo const& info);
```

Delegates to `printStringToStream()`.

### printIntToStream()

Defined in `printIntToStream.h`. Handles integer formatting with full `printf`-compatible width and precision semantics.

```cpp
template<typename T>
void printIntToStream(std::ostream& s, T arg, FormatInfo const& info);
```

**Algorithm:**

1. If no width or precision specified, simply calls `s << arg`.
2. Otherwise, manually constructs the output:
   a. Computes `numberOfDigits` using `log10()` with the appropriate base (8, 10, or 16).
   b. Computes padding (spaces) and prefix zeros from width and precision.
   c. If `info.leftPad` (`0` flag) and no explicit precision, swaps prefix zeros and padding.
   d. Outputs: padding spaces (if right-justified) + sign character + base prefix (`0`, `0x`, `0X`) + prefix zeros + absolute value + padding spaces (if left-justified).
   e. Special case: precision 0 with value 0 prints nothing for the digit part.

Uses `absm()` helper overloads for getting the absolute value of signed/unsigned integers.

### printStringToStream()

Defined in `printStringToStream.h`. Handles string formatting with precision-based truncation.

```cpp
void printStringToStream(std::ostream& s, char const* const& arg, FormatInfo const& info);
```

**Algorithm:**

1. If no precision specified, simply calls `s << arg`.
2. Otherwise, resets `s.width(0)` and manually outputs:
   a. Padding spaces (if right-justified and precision < width).
   b. Up to `info.precision` characters from `arg` (truncating the string).
   c. Padding spaces (if left-justified).

---

## Dynamic Width/Precision Storage

### saveToStream()

Defined in `saveToStream.h`. Stores a dynamic width or precision value in the stream's `iword()` slot for later retrieval by `applyData()`.

```cpp
// Generic no-op (non-int types)
template<typename T>
void saveToStream(std::ostream&, Dynamic, T const&) {}

// int specialization: stores the value
template<>
void saveToStream(std::ostream& s, Dynamic pos, int const& size)
{
    s.iword(static_cast<int>(pos)) = size;
}
```

The `Dynamic` enum value is used as the `iword` index:
- `Dynamic::Width` (1) stores the dynamic width.
- `Dynamic::Precision` (2) stores the dynamic precision.

`applyData()` reads these back via `s.iword(static_cast<int>(Dynamic::Width))` and `s.iword(static_cast<int>(Dynamic::Precision))`. A negative dynamic width implies left-justification (matching `printf` behavior).

---

## Utility Functions

Defined in `Utility.h`. **Namespace:** `ThorsAnvil::Utility`.

### print()

```cpp
template<typename... Args>
void print(std::ostream& s, Args&... args);
```

Streams all arguments to `s` in order using a pack expansion with a dummy array.

### buildStringFromParts()

```cpp
template<typename... Args>
std::string buildStringFromParts(Args const&... args);
```

Concatenates all arguments into a single string via `std::stringstream`. Used internally for building error messages and by ThorsLogging.

### buildErrorMessage()

```cpp
template<typename... Args>
std::string buildErrorMessage(char const* className, char const* method, Args const&... args);
```

Builds a formatted error message: `"id: <N> <className>::<method>: <args...>"`. Each call increments a global counter via `getUniqueErrorId()`. Used by ThorsLogging macros.

### buildBugReport()

```cpp
template<typename... Args>
std::string buildBugReport(Args const&... a);
```

Calls `buildErrorMessage()` and appends `"\nPlease File a Bug Report: "`.

### getUniqueErrorId()

```cpp
std::size_t getUniqueErrorId();
```

Returns a monotonically increasing integer. Thread-unsafe (static local counter). Used to give each error message a unique identifier.

### systemErrorMessage()

```cpp
std::string systemErrorMessage();
```

Returns a human-readable string for the current `errno` value. Calls `errnoToName()` to convert the errno constant to its name (e.g., `"ECONNRESET "`), then appends the system's `strerror()` message. Handles Windows via `strerror_s()`.

### errnoToName()

```cpp
std::string errnoToName();
```

A large `switch` statement mapping errno constants (`EACCES`, `EADDRINUSE`, `EAGAIN`, `EBADF`, `ECONNRESET`, `EINTR`, `EINVAL`, `EPIPE`, `ETIMEDOUT`, etc.) to their string names. Returns `"Unknown: "` for unrecognized values. Conditionally excludes `EDQUOT` on Windows.

---

## StreamFormatterNoChange Class

Defined in `Utility.h`. **Namespace:** `ThorsAnvil::Utility`.

An RAII guard that preserves a stream's format state across an expression.

```cpp
class StreamFormatterNoChange
{
    mutable std::ios*               stream;
    mutable std::ios_base::fmtflags flags;
    mutable std::streamsize         precision;
public:
    StreamFormatterNoChange();
    ~StreamFormatterNoChange();
    void saveStream(std::ios& s) const;

    friend std::ostream& operator<<(std::ostream&, StreamFormatterNoChange const&);
    friend std::istream& operator>>(std::istream&, StreamFormatterNoChange const&);
};
```

**Usage:**

```cpp
stream << StreamFormatterNoChange{} << std::hex << std::setprecision(23) << value;
// Stream flags and precision are restored after this expression
```

**How it works:** The `operator<<` stores a pointer to the stream and saves its current `flags()` and `precision()`. The temporary `StreamFormatterNoChange` object is destroyed at the end of the full expression (`;`), and the destructor restores the saved state.

---

## Formatting Pipeline

End-to-end flow for `std::cout << make_format("Hello %s, you are %d years old.\n", name, age)`:

```
make_format("Hello %s, you are %d years old.\n", name, age)
  │
  ▼
Format<char const*, int> constructor
  ├─ getNextPrefix() → prefixString[0] = "Hello "
  ├─ Formatter("%s", None) → formater[0] (type=String, specifier=s)
  ├─ getNextPrefix() → prefixString[1] = ", you are "
  ├─ Formatter("%d", None) → formater[1] (type=Int, specifier=d)
  └─ getNextPrefix() → prefixString[2] = " years old.\n"
  │
  ▼
operator<<(cout, format) → format.print(cout)
  │
  ▼
doPrint(cout, index_sequence<0, 1>)
  ├─ printValue<0>(cout)
  │   ├─ cout << "Hello "
  │   ├─ cout << formater[0]  → FormatterCheck{cout, formater[0]}
  │   └─ FormatterCheck << name → formater[0].apply(cout, name)
  │       ├─ typeid(char const*) == expectedType? → Yes
  │       └─ applyData(cout, name)
  │           ├─ save stream state
  │           ├─ set flags (right-justified)
  │           ├─ printToStream(cout, name, info) → printStringToStream()
  │           └─ restore stream state
  ├─ printValue<1>(cout)
  │   ├─ cout << ", you are "
  │   ├─ cout << formater[1]  → FormatterCheck{cout, formater[1]}
  │   └─ FormatterCheck << age → formater[1].apply(cout, age)
  │       ├─ typeid(int) == expectedType? → Yes
  │       └─ applyData(cout, age)
  │           ├─ save stream state
  │           ├─ set flags (dec, right-justified)
  │           ├─ printToStream(cout, age, info) → printIntToStream()
  │           └─ restore stream state
  └─ cout << " years old.\n"
```

---

## Type Resolution Table

The static map in `Formatter::getType()` resolves `(Type, Length)` pairs to expected C++ types:

### Integer Types (Type::Int)

| Length | Expected C++ Type | Truncation Mask |
|--------|-------------------|-----------------|
| `none` | `int` | 0 |
| `hh` | `char` | 0xFF |
| `h` | `short int` | 0xFFFF |
| `l` | `long int` | 0 |
| `ll` | `long long int` | 0 |
| `j` | `std::intmax_t` | 0 |
| `z` | `std::size_t` | 0 |
| `t` | `std::ptrdiff_t` | 0 |

### Unsigned Integer Types (Type::UInt)

| Length | Expected C++ Type | Truncation Mask |
|--------|-------------------|-----------------|
| `none` | `unsigned int` | 0 |
| `hh` | `unsigned char` | 0xFF |
| `h` | `unsigned short int` | 0xFFFF |
| `l` | `unsigned long int` | 0 |
| `ll` | `unsigned long long int` | 0 |
| `j` | `std::intmax_t` | 0 |
| `z` | `std::size_t` | 0 |
| `t` | `std::ptrdiff_t` | 0 |

### Floating-Point Types (Type::Float)

| Length | Expected C++ Type |
|--------|-------------------|
| `none` / `l` | `double` |
| `L` | `long double` |

### Other Types

| Type | Length | Expected C++ Type |
|------|--------|-------------------|
| Char | `none` | `int` |
| Char | `l` | `std::wint_t` |
| String | `none` | `char const*` |
| String | `l` | `wchar_t const*` |
| Pointer | `none` | `void*` |
| Count | `none` | `int*` |
| Count | `hh` | `signed char*` |
| Count | `h`/`l`/`ll`/`j`/`z`/`t` | corresponding pointer types |

Any `(Type, Length)` combination not in this table throws `std::invalid_argument`.

---

## Error Handling

All errors throw `std::invalid_argument`:

| Condition | Message |
|-----------|---------|
| More arguments than `%`-specifiers | `"Invalid Format: not enough format specifiers for provided arguments"` |
| More `%`-specifiers than arguments | `"Invalid Format: too many format specifiers for provided arguments"` |
| Unrecognized specifier character | `"Invalid Parameter specifier: <char>"` |
| Invalid `(Type, Length)` combination | `"Specifier and length are not a valid combination"` |
| Argument type mismatch | `"Actual argument does not match supplied argument (or conversions): Expected(<name>) Got(<name>)"` |
| Dynamic `*` argument is not `int` | `"Dynamic Width of Precision is not an int"` |
