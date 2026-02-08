# ThorsIOUtil Internal Documentation

Implementation details for the ThorsIOUtil formatting library.

**Source:** `third/ThorsIOUtil/src/ThorsIOUtil/`

---

## Formatting Pipeline

When you write:

```cpp
std::cout << make_format("Hello %s, you are %d years old.\n", name, age);
```

The following steps occur:

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

## Strictness Modes

| Mode | Description |
|------|-------------|
| `CTypeCompat` | Allows C-style implicit integer promotions (e.g., `short` matches `%d`) |
| `CPPTypeStrict` | Requires exact type matches between argument and specifier |

---

## Format String Parsing

The parser walks the format string character by character:
1. Copies literal characters into the current prefix string.
2. On `%`:
   - `%%` is treated as a literal `%`.
   - Otherwise, parses: flags (`-`, `+`, ` `, `#`, `0`), width (`*` or digits), precision (`.` + `*` or digits), length modifier (`h`, `hh`, `l`, `ll`, `z`, `j`, `t`, `L`), and conversion specifier.
3. Creates a `Formatter` object capturing all parsed fields.

---

## Formatter Class

Each `%`-specifier is represented as a `Formatter` with:
- `flags`: formatting flags
- `width`: field width (-1 for dynamic `*`)
- `precision`: decimal precision (-1 for dynamic `*`)
- `lengthModifier`: the parsed length modifier
- `specifier`: the conversion character

### apply() Method

1. Checks the argument type against the expected type for the specifier + length modifier combination.
2. In `CTypeCompat` mode, applies C promotion rules (e.g., `short` and `char` are valid for `%d`).
3. In `CPPTypeStrict` mode, requires exact match.

### applyData() Method

1. Saves the current stream state (`flags`, `fill`, `width`, `precision`).
2. Configures the stream based on the formatter's settings:
   - `-` flag → `std::left`
   - `+` flag → `std::showpos`
   - `#` flag → `std::showbase` / `std::showpoint`
   - `0` flag → `std::setfill('0')` with `std::internal`
   - Width and precision via `std::setw()` and `std::setprecision()`
3. Calls `printToStream()`.
4. Restores the original stream state.

### printToStream() Dispatch

- Integer specifiers (`d`, `i`, `u`, `x`, `X`, `o`): sets `std::dec`, `std::hex`, or `std::oct`, then uses `operator<<`.
- String specifier (`s`): outputs the string, applying width and precision.
- Character specifier (`c`): outputs a single character.
- Floating point (`f`, `e`, `g`, `a`): sets `std::fixed`, `std::scientific`, etc.
- Pointer (`p`): casts to `void*` and outputs.

---

## Error Handling

All errors throw `std::invalid_argument`:

- **Too few specifiers:** more arguments than `%`-specifiers.
- **Too many specifiers:** more `%`-specifiers than arguments.
- **Invalid specifier character:** unrecognized character after length modifier.
- **Invalid length + specifier combination:** e.g., `%hhf`.
- **Type mismatch:** argument type does not match expected type (and no valid conversion exists under the current `Strictness` mode).
- **Dynamic width/precision not `int`:** `*` arguments must be `int`.

---

## Additional Utilities

The library also provides stream helpers in `ThorsAnvil::Utility`:

- `buildErrorMessage(...)` -- concatenates variadic arguments via `std::ostringstream`. Used by ThorsLogging.
- `buildStringFromParts(...)` -- similar utility for building strings from parts.
