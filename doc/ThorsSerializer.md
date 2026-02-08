# ThorsSerializer Developer Guide

A declarative C++ serialization library for JSON, YAML, and BSON. Instead of manually building a DOM, you declare which members of your C++ types should be serialized, and ThorsSerializer generates the appropriate code at compile time.

---

## Table of Contents

1. [Installation](#installation)
2. [Quick Start](#quick-start)
3. [Core Concepts](#core-concepts)
4. [Header Includes](#header-includes)
5. [Declaring Serializable Types](#declaring-serializable-types)
   - [Simple Structs](#simple-structs)
   - [Classes with Private Members](#classes-with-private-members)
   - [Inheritance Hierarchies](#inheritance-hierarchies)
   - [Template Types](#template-types)
6. [Serialization (Writing)](#serialization-writing)
7. [Deserialization (Reading)](#deserialization-reading)
8. [Format-Specific APIs](#format-specific-apis)
   - [JSON](#json)
   - [YAML](#yaml)
   - [BSON](#bson)
9. [Configuration](#configuration)
   - [PrinterConfig (Export)](#printerconfig-export)
   - [ParserConfig (Import)](#parserconfig-import)
10. [Supported Standard Library Types](#supported-standard-library-types)
11. [Enum Handling](#enum-handling)
12. [Field Name Overrides](#field-name-overrides)
13. [Optional / Filtered Fields](#optional--filtered-fields)
14. [Pointer and Smart Pointer Handling](#pointer-and-smart-pointer-handling)
15. [Polymorphic Serialization](#polymorphic-serialization)
16. [Custom Serialization](#custom-serialization)
17. [Builder Functions](#builder-functions)
18. [Range Serialization](#range-serialization)
19. [Complete Examples](#complete-examples)
20. [Class Reference](#class-reference)
21. [Macro Reference](#macro-reference)
22. [Build Instructions](#build-instructions)

---

## Installation

### Homebrew (Mac and Linux)

```bash
brew install thors-anvil
```

### Header-Only (from GitHub)

```bash
git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsMongo.git
```

Add the cloned directory to your include path. Dependencies:
- **All formats**: [magic_enum](https://github.com/Neargye/magic_enum) (for automatic enum handling)
- **YAML**: [libyaml](https://github.com/yaml/libyaml)
- **JSON**: None

### From Source

```bash
git clone https://github.com/Loki-Astari/ThorsSerializer.git
cd ThorsSerializer
./configure
make
sudo make install
```

Requires a C++20 compatible compiler.

---

## Quick Start

Serialize a `std::vector<int>` to/from JSON in under 10 lines:

```cpp
#include <iostream>
#include <vector>
#include "ThorSerialize/JsonThor.h"

int main()
{
    using ThorsAnvil::Serialize::jsonImporter;
    using ThorsAnvil::Serialize::jsonExporter;

    std::vector<int> data;
    std::cin >> jsonImporter(data);           // read JSON from stdin
    std::cout << jsonExporter(data) << "\n";  // write JSON to stdout
}
```

```bash
g++ -std=c++20 quick.cpp -lThorSerialize -lThorsLogging
echo "[1,2,3,4,5]" | ./a.out
# Output: [1, 2, 3, 4, 5]
```

---

## Core Concepts

ThorsSerializer works on a **traits-based** system:

1. **`Traits<T>`** -- A template specialization that tells the serializer the shape of type `T` (which members to read/write). You declare these using macros.
2. **Exporter / Importer** -- Lightweight wrapper objects returned by `jsonExporter()`, `yamlExporter()`, `bsonExporter()` (and their `*Importer` counterparts). These integrate with `operator<<` / `operator>>`.
3. **Serializer / DeSerializer** -- The internal engine that walks the `Traits<T>` metadata and drives the printer/parser.
4. **PrinterInterface / ParserInterface** -- Format-specific backends (JSON, YAML, BSON) that handle the actual byte-level I/O.

All serialization metadata is resolved at compile time. The runtime overhead is minimal.

---

## Header Includes

| Header | Purpose |
|--------|---------|
| `ThorSerialize/Traits.h` | Core `Traits<T>` class and all declaration macros (`ThorsAnvil_MakeTrait`, etc.) |
| `ThorSerialize/SerUtil.h` | `Traits<T>` specializations for all standard containers (`std::vector`, `std::map`, `std::optional`, `std::variant`, etc.) |
| `ThorSerialize/JsonThor.h` | JSON exporter/importer: `jsonExporter()`, `jsonImporter()` |
| `ThorSerialize/YamlThor.h` | YAML exporter/importer: `yamlExporter()`, `yamlImporter()` |
| `ThorSerialize/BsonThor.h` | BSON exporter/importer: `bsonExporter()`, `bsonImporter()` |

Typical include order for a header that declares serializable types:

```cpp
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"    // if using std:: containers
#include "ThorSerialize/JsonThor.h"   // if serializing to JSON
```

---

## Declaring Serializable Types

### Simple Structs

Use `ThorsAnvil_MakeTrait()` at **global scope** (outside any namespace) to declare which members are serializable:

```cpp
struct Color
{
    int red;
    int green;
    int blue;
};

// Must be at global scope, even if Color is in a namespace
ThorsAnvil_MakeTrait(Color, red, green, blue);
```

If `Color` were in namespace `Graphics`:

```cpp
ThorsAnvil_MakeTrait(Graphics::Color, red, green, blue);
```

### Classes with Private Members

The serialization code has no special access to your class. Declare `Traits<T>` as a friend:

```cpp
namespace MyApp
{
    class User
    {
        // Grant serializer access to private members
        friend class ThorsAnvil::Serialize::Traits<User>;

        std::string name;
        int         age;
    public:
        User() : name(""), age(0) {}
        User(std::string n, int a) : name(std::move(n)), age(a) {}
    };
}

ThorsAnvil_MakeTrait(MyApp::User, name, age);
```

### Inheritance Hierarchies

Use `ThorsAnvil_ExpandTrait()` when a class extends a serializable base class. The parent's fields are automatically included:

```cpp
struct Base
{
    int value1;
    int value2;
};

struct Derived : public Base
{
    int value3;
    int value4;
};

ThorsAnvil_MakeTrait(Base, value1, value2);
ThorsAnvil_ExpandTrait(Base, Derived, value3, value4);
// Serializing Derived will include value1, value2, value3, value4
```

### Template Types

For template classes, use `ThorsAnvil_Template_MakeTrait()` with the template parameter count as the first argument:

```cpp
template<typename T>
struct Wrapper
{
    T data;
};

ThorsAnvil_Template_MakeTrait(1, Wrapper, data);

template<typename K, typename V>
struct Pair
{
    K key;
    V val;
};

ThorsAnvil_Template_MakeTrait(2, Pair, key, val);
```

For template types with inheritance:

```cpp
template<typename T>
struct DerivedWrapper : public Base
{
    T extra;
};

ThorsAnvil_Template_ExpandTrait(1, Base, DerivedWrapper, extra);
```

---

## Serialization (Writing)

Use the `<format>Exporter()` function with `operator<<`:

```cpp
#include "ThorSerialize/JsonThor.h"

MyType obj;
// ... populate obj ...

// Write to stdout
std::cout << ThorsAnvil::Serialize::jsonExporter(obj);

// Write to a string
std::string output;
output << ThorsAnvil::Serialize::jsonExporter(obj);
```

---

## Deserialization (Reading)

Use the `<format>Importer()` function with `operator>>`:

```cpp
#include "ThorSerialize/JsonThor.h"

MyType obj;

// Read from stdin
std::cin >> ThorsAnvil::Serialize::jsonImporter(obj);

// Read from a string
std::string input = R"({"name": "Alice", "age": 30})";
input >> ThorsAnvil::Serialize::jsonImporter(obj);

// Read from a string_view
std::string_view sv = R"({"name": "Bob", "age": 25})";
sv >> ThorsAnvil::Serialize::jsonImporter(obj);
```

---

## Format-Specific APIs

### JSON

```cpp
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Serialize;

// Export (serialize)
std::cout << jsonExporter(myObj);
std::cout << jsonExporter(myObj, PrinterConfig{}.setOutputType(OutputType::Stream));

// Import (deserialize)
std::cin >> jsonImporter(myObj);
std::cin >> jsonImporter(myObj, ParserConfig{}.setParseStrictness(ParseType::Strict));

// Builder: construct object from stream in one call
auto obj = jsonBuilder<MyType>(std::cin);
auto obj2 = jsonBuilder<MyType>(std::string{R"({"key":"value"})"});

// Get serialized size (compressed/stream format)
std::size_t size = jsonStreanSize(myObj);
```

### YAML

```cpp
#include "ThorSerialize/YamlThor.h"

using namespace ThorsAnvil::Serialize;

std::cout << yamlExporter(myObj);
std::cin  >> yamlImporter(myObj);

auto obj = yamlBuilder<MyType>(std::cin);
```

### BSON

```cpp
#include "ThorSerialize/BsonThor.h"

using namespace ThorsAnvil::Serialize;

std::cout << bsonExporter(myObj);
std::cin  >> bsonImporter(myObj);

auto obj = bsonBuilder<MyType>(std::cin);

// Get the BSON byte size of an object
std::size_t size = bsonGetPrintSize(myObj);
```

---

## Configuration

### PrinterConfig (Export)

The second optional argument to any `<format>Exporter()` function:

```cpp
PrinterConfig config;
config.setOutputType(OutputType::Config)       // Human-readable formatting
      .setPolymorphicMarker("MyType")          // Field name for polymorphic type info
      .setCatchExceptions(true);               // Catch exceptions deirived from std::exception and set failbit
      .setCatchUnknownExceptions(true)         // Catch any other exceptions.
                                               // Don't set this to true unless you know what you are doing.  
                                               // This will cause issues with boost-coroutines.
      .setUseOldSharedPtr()                    // shared_ptr is duplicated while serializing. 
                                               // Set to true to use the old technique, which is not good.
      .setExactPreFlightCalc()                 // When serializing to strings, pre-calculate the exact string outputsize.
      .setTabSize(2)                           // Set the indent size.
      .setBlockSize(1);                        // Indent from the left margin.


std::cout << jsonExporter(myObj, config);
```

| Method | Values | Default | Description |
|--------|--------|---------|-------------|
| `setOutputType()` | `Default`, `Stream`, `Config` | `Default` | `Stream` = compressed, `Config` = pretty-printed. Only affects JSON. |
| `setPolymorphicMarker()` | any string | `"__type"` | Key name used for polymorphic type identification |
| `setCatchExceptions()` | `true` / `false` | `true` | If `true`, exceptions derived from std::exceptions set the stream failbit instead of propagating |
| `setCatchUnknownExceptions()` | `true` / `false` | false | if `true`, any other exceptions set the stream failbit instead of propagating |
| `setUseOldSharedPtr()` | `true` / `false` | `false` | Use legacy shared_ptr serialization (no dedup) |
│ `setExactPreFlightCalc()` | `true` / `false` | `false` | Set to true to not pre-flight. Otherwise, pre-flight the output to calculate the string size needed. |
| `setTabSize()` | int | 4 &lt;JSON&gt;<br>2 &lt;YAML&gt; | The tab indent size. |
| `setBlockSize()` | int | 0 | The indent from left side. |


### ParserConfig (Import)

The second argument to any `*Importer()` function:

```cpp
auto dynamcClassDetector = [](DataInputStream&){return "Type";};
IgnoreCallBack logMissedBsonData;

ParserConfig config;
config.setParseStrictness(ParseType::Exact)         // How strict the parsing is.
      .setPolymorphicMarker("MyType")               // Field name for polymorphic type info.
      .setCatchExceptions(false)                    // Catch exceptions deirived from std::exception and set failbit
      .setCatchUnknownExceptions(false)             // Catch any other exceptions.
                                                    // Don't set this to true unless you know what you are doing.  
                                                    // This will cause issues with boost-coroutines.
      .setUseOldSharedPtr(false)                    // shared_ptr is duplicated while serializing. 
                                                    // Set to true to use the old technique, which is not good.
      .setValidateNoTrailingData(true)              // Set to true to validate that stream ends after data.
      .setNoBackslashConversion(false)              // Don't convert backslash in string
      .setIdentifyDynamicClass(dynamcClassDetector) // Function that will return the class name
      .setIgnoreCallBack(logMissedBsonData);        // Used to debug BSON objects.

std::cin >> jsonImporter(myObj, config);
```

| Method | Values | Default | Description |
|--------|--------|---------|-------------|
| `setParseStrictness()` | `Weak`, `Strict`, `Exact` | `Weak` | `Weak`: ignore unknown fields.<br>`Strict`: unknown fields throw.<br>`Exact`: unknown AND missing fields throw. |
| `setPolymorphicMarker()` | any string | `"__type"` | Key name for polymorphic type identification |
| `setCatchExceptions()` | `true` / `false` | `true` | If `true`, exceptions set the stream failbit instead of propagating |
| `setCatchUnknownExceptions()` | `true` / `false` | `false` | if `true`, any other exceptions set the stream failbit instead of propagating |
| `setValidateNoTrailingData()` | `true` / `false` | `false` | If `true`, fail if there is non-whitespace data after the parsed object |
| `setUseOldSharedPtr()` | `true` / `false` | `false` | If `true` use legacy shared_ptr deserialization |
│ `setNoBackslashConversion()` | `true` / `false` | `true` | If `true`, don't convert backslash characters. |
| `setIdentifyDynamicClass()` | `std::function<std::string(Stream)>` | Does nothing | Get polymorphic class name when marker is not available as the first key |
| `setIgnoreCallBack()` | `IgnoreCallBack` | Does nothing | Log unused BSON data |

## Supported Standard Library Types

Include `ThorSerialize/SerUtil.h` to get automatic `Traits<T>` for these types:

| Type | Serialized As |
|------|---------------|
| `std::vector<T>` | array |
| `std::list<T>` | array |
| `std::deque<T>` | array |
| `std::array<T, N>` | array |
| `std::set<T>` | array |
| `std::multiset<T>` | array |
| `std::unordered_set<T>` | array |
| `std::unordered_multiset<T>` | array |
| `std::tuple<Args...>` | array |
| `std::initializer_list<T>` | array (export only) |
| `std::pair<F, S>` | object with keys `"first"`, `"second"` |
| `std::map<std::string, V>` | object (string keys become JSON keys) |
| `std::map<K, V>` | array of pairs (non-string keys) |
| `std::unordered_map<std::string, V>` | object |
| `std::unordered_map<K, V>` | array of pairs |
| `std::multimap<std::string, V>` | object |
| `std::unique_ptr<T>` | Value of `T` if non-null, `null` otherwise |
| `std::shared_ptr<T>` | Deduplicated: serialized once, referenced by ID thereafter |
| `std::optional<T>` | Value of `T` if has value; **omitted entirely** if empty |
| `std::variant<Args...>` | Serialized as the active alternative's type |
| `std::reference_wrapper<T>` | Serialized as the referenced `T` |

**Primitive types** (`int`, `long`, `double`, `bool`, `std::string`, etc.) are handled automatically by the framework.

---

## Enum Handling

Enums are handled automatically via [magic_enum](https://github.com/Neargye/magic_enum). Enum values are serialized as their string names:

```cpp
enum class Color { Red, Green, Blue };

struct Shirt
{
    Color color;
};

ThorsAnvil_MakeTrait(Shirt, color);
// Serializes as: {"color": "Red"}
```

No extra macro is needed when using `magic_enum`.

### Legacy Enum Declaration (Deprecated)

For explicit control or if `magic_enum` is unavailable, use `ThorsAnvil_MakeEnum`:

```cpp
enum TransType { Error, Deposit, Withdraw, Correction };
ThorsAnvil_MakeEnum(TransType, Error, Deposit, Withdraw, Correction);
```

---

## Field Name Overrides

C++ identifiers cannot contain characters like `$` or start with digits, but serialization keys sometimes require them. Use `ThorsAnvil_MakeOverride()` **before** the corresponding `ThorsAnvil_MakeTrait()`:

```cpp
struct MongoQuery
{
    std::string database;
    std::string collection;
};

// Map C++ field "database" -> JSON key "$db"
// Map C++ field "collection" -> JSON key "$coll"
ThorsAnvil_MakeOverride(MongoQuery, {"database", "$db"}, {"collection", "$coll"});
ThorsAnvil_MakeTrait(MongoQuery, database, collection);
```

Produces:

```json
{"$db": "mydb", "$coll": "users"}
```

For template classes, use `ThorsAnvil_Template_MakeOverride(Count, ...)`.

---

## Optional / Filtered Fields

### Using `std::optional` (Recommended)

Fields declared as `std::optional<T>` are automatically omitted from output when they have no value:

```cpp
struct UserProfile
{
    std::string             name;
    std::optional<int>      age;       // omitted if empty
    std::optional<std::string> email;  // omitted if empty
};

ThorsAnvil_MakeTrait(UserProfile, name, age, email);
```

```cpp
UserProfile u{"Alice", std::nullopt, "alice@example.com"};
std::cout << jsonExporter(u);
// Output: {"name": "Alice", "email": "alice@example.com"}
// Note: "age" is completely absent
```

### Legacy Filter (Deprecated)

`ThorsAnvil_MakeFilter(ClassName, filterField)` is maintained for backward compatibility. Prefer `std::optional`.

---

## Pointer and Smart Pointer Handling

### Raw Pointers

When a member is a raw pointer (`T*`), serialization outputs `null` for `nullptr`, or the pointed-to value otherwise. Deserialization allocates with `new T{}` and deallocates with `delete`.

### `std::unique_ptr<T>`

Works like a raw pointer: outputs `null` for empty, otherwise serializes the owned object.

### `std::shared_ptr<T>`

Shared pointers are **deduplicated**. If two `shared_ptr` fields point to the same object, the object is serialized once and referenced by ID on subsequent occurrences. Deserialization correctly restores the shared ownership.

### Custom Allocation

Override the default `new`/`delete` behavior:

```cpp
struct NodeAllocator
{
    static Node* alloc()             { return myPool.allocate(); }
    static void  release(Node* ptr)  { myPool.deallocate(ptr); }
};

ThorsAnvil_MakeTrait(Node, value, left, right);
ThorsAnvil_PointerAllocator(Node, NodeAllocator);
```

---

## Polymorphic Serialization

When deserializing a pointer to a base class, the library needs to know the concrete type. ThorsSerializer embeds a type discriminator field (default `"__type"`) in the serialized output.

### Step 1: Add the Macro Inside the Class

```cpp
struct Transport
{
    int wheelCount;
    virtual ~Transport() = default;

    // Add in a public section of the base class
    ThorsAnvil_PolyMorphicSerializer(Transport);
};

struct Car : public Transport
{
    std::string engineType;

    // Use WithOverride for derived classes (adds `override` keyword)
    ThorsAnvil_PolyMorphicSerializerWithOverride(Car);
};

struct Bike : public Transport
{
    bool electric;

    ThorsAnvil_PolyMorphicSerializerWithOverride(Bike);
};
```

### Step 2: Declare Traits Normally

```cpp
ThorsAnvil_MakeTrait(Transport, wheelCount);
ThorsAnvil_ExpandTrait(Transport, Car, engineType);
ThorsAnvil_ExpandTrait(Transport, Bike, electric);
```

### Step 3: Use with Pointer Members

```cpp
struct Fleet
{
    std::string name;
    std::vector<std::unique_ptr<Transport>> vehicles;
};

ThorsAnvil_MakeTrait(Fleet, name, vehicles);
```

Serialized JSON:

```json
{
    "name": "CityFleet",
    "vehicles": [
        {"__type": "Car", "wheelCount": 4, "engineType": "V8"},
        {"__type": "Bike", "wheelCount": 2, "electric": true}
    ]
}
```

### Custom Type Names

By default the C++ type name is used. To use a custom name:

```cpp
ThorsAnvil_RegisterPolyMorphicTypeNamed(MyNamespace::Car, "automobile");
```

### Custom Type Field Name

Override the `"__type"` field globally via config, or per-class:

```cpp
// Per-class: add inside the class definition
ThorsAnvil_TypeFieldName(kind);  // uses "kind" instead of "__type"

// Per-operation: via config
PrinterConfig{}.setPolymorphicMarker("__class");
```

---

## Custom Serialization

When the standard member-by-member approach is not sufficient, implement a custom serializer:

### Step 1: Define a Custom Serializer Type

```cpp
struct MyTypeSerializer
{
    // Required for BSON
    static std::size_t getPrintSizeBson(
        ThorsAnvil::Serialize::BsonPrinter& printer,
        MyType const& object);

    // Called for all text-based formats (JSON, YAML)
    static void writeCustom(
        ThorsAnvil::Serialize::PrinterInterface& printer,
        MyType const& object);

    static void readCustom(
        ThorsAnvil::Serialize::ParserInterface& parser,
        MyType& object);
};
```

### Step 2: Register It

```cpp
ThorsAnvil_MakeTraitCustomSerialize(MyType, MyTypeSerializer);
```

### Using `DefaultCustomSerializer<T>` Base Class

For format-specific overrides, inherit from `DefaultCustomSerializer<T>`:

```cpp
struct IDSerializer : public ThorsAnvil::Serialize::DefaultCustomSerializer<ID>
{
    // Override only the formats you need:
    virtual void writeJson(JsonPrinter& printer, ID const& obj) override
    {
        printer.stream() << obj.value;
    }
    virtual void readJson(JsonParser& parser, ID& obj) override
    {
        parser.stream() >> obj.value;
    }
    // BSON requires size information:
    virtual std::size_t getPrintSizeBson(BsonPrinter&, ID const&) override
    {
        return 12;
    }
    virtual char getBsonByteMark() override { return '\x07'; }
    virtual void writeBson(BsonPrinter& printer, ID const& obj) override { /* ... */ }
    virtual void readBson(BsonParser& parser, char mark, ID& obj) override { /* ... */ }
};

ThorsAnvil_MakeTraitCustomSerialize(ID, IDSerializer);
```

---

## Builder Functions

Construct an object from a stream in a single expression:

```cpp
// From std::istream
auto user = ThorsAnvil::Serialize::jsonBuilder<User>(std::cin);

// From std::string
auto user = ThorsAnvil::Serialize::jsonBuilder<User>(
    std::string{R"({"name":"Alice","age":30})"});

// YAML / BSON equivalents
auto user = ThorsAnvil::Serialize::yamlBuilder<User>(yamlStream);
auto user = ThorsAnvil::Serialize::bsonBuilder<User>(bsonStream);
```

Returns a default-constructed `T{}` if parsing fails.

---

## Range Serialization

You can serialize any C++ range (that does not already have `Traits` defined) as an array:

```cpp
#include <ranges>
#include <vector>

std::vector<int> data = {1, 2, 3, 4, 5};

// Serialize a subrange
auto view = data | std::views::filter([](int x){ return x > 2; });
std::cout << ThorsAnvil::Serialize::jsonExporter(view);
// Output: [3, 4, 5]
```

This works with `jsonExporter`, `yamlExporter`, and `bsonExporter`.

---

## Complete Examples

### Example: Nested Structs with JSON

```cpp
#include <iostream>
#include <sstream>
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"

struct Shirt
{
    int red;
    int green;
    int blue;
};

class TeamMember
{
    friend class ThorsAnvil::Serialize::Traits<TeamMember>;
    std::string name;
    int         score;
    int         damage;
    Shirt       team;
public:
    TeamMember() : name(""), score(0), damage(0), team{0,0,0} {}
    TeamMember(std::string n, int s, int d, Shirt t)
        : name(std::move(n)), score(s), damage(d), team(t) {}
};

ThorsAnvil_MakeTrait(Shirt, red, green, blue);
ThorsAnvil_MakeTrait(TeamMember, name, score, damage, team);

int main()
{
    using ThorsAnvil::Serialize::jsonExporter;
    using ThorsAnvil::Serialize::jsonImporter;

    // Serialize
    TeamMember mark("Mark", 10, 5, {255, 0, 0});
    std::cout << jsonExporter(mark) << "\n";

    // Deserialize
    TeamMember john;
    std::stringstream input(
        R"({"name":"John","score":13,"damage":2,"team":{"red":0,"green":0,"blue":255}})");
    input >> jsonImporter(john);
    std::cout << jsonExporter(john) << "\n";
}
```

Output:

```json
{
    "name": "Mark",
    "score": 10,
    "damage": 5,
    "team": {
        "red": 255,
        "green": 0,
        "blue": 0
    }
}
{
    "name": "John",
    "score": 13,
    "damage": 2,
    "team": {
        "red": 0,
        "green": 0,
        "blue": 255
    }
}
```

### Example: Polymorphic Bank Account

```cpp
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include "ThorSerialize/JsonThor.h"
#include <vector>
#include <memory>

struct Transaction
{
    long timeStamp;
    int  amount;
};

class BankAccount
{
    friend class ThorsAnvil::Serialize::Traits<BankAccount>;
    int         balance;
    std::string details;
public:
    BankAccount() : balance(0) {}
    virtual ~BankAccount() = default;
    ThorsAnvil_PolyMorphicSerializer(BankAccount);
};

class CurrentAccount : public BankAccount
{
    friend class ThorsAnvil::Serialize::Traits<CurrentAccount>;
    std::vector<Transaction> actions;
public:
    CurrentAccount() = default;
    ThorsAnvil_PolyMorphicSerializerWithOverride(CurrentAccount);
};

ThorsAnvil_MakeTrait(Transaction, timeStamp, amount);
ThorsAnvil_MakeTrait(BankAccount, balance, details);
ThorsAnvil_ExpandTrait(BankAccount, CurrentAccount, actions);
```

---

## Class Reference

### `ThorsAnvil::Serialize::Serializer`

The internal serialization engine. Normally used indirectly through exporters.

| Method | Description |
|--------|-------------|
| `Serializer(PrinterInterface&, bool root = true)` | Construct with a printer; `root=true` emits doc start/end markers |
| `void print(T const& object)` | Serialize object `T` |
| `void printObjectMembers(T const& object)` | Print only the members (no open/close) |

### `ThorsAnvil::Serialize::DeSerializer`

The internal deserialization engine. Normally used indirectly through importers.

| Method | Description |
|--------|-------------|
| `DeSerializer(ParserInterface&, bool root = true)` | Construct with a parser |
| `void parse(T& object)` | Deserialize into object `T` |

### `ThorsAnvil::Serialize::Traits<T>`

Template class specialized for each serializable type. Key members:

| Member | Description |
|--------|-------------|
| `static constexpr TraitType type` | The trait category: `Map`, `Array`, `Value`, `Enum`, `Pointer`, `Parent`, etc. |
| `static Members const& getMembers()` | Returns a tuple of `(name, member-pointer)` pairs |
| `using Parent` | (If `ExpandTrait`) The parent type |

### TraitType Enum

| Value | Meaning |
|-------|---------|
| `Invalid` | No traits defined (compile error if serialized) |
| `Map` | Object with named fields (structs/classes) |
| `Array` | Ordered sequence (vectors, lists, etc.) |
| `Parent` | Object that extends a serializable parent |
| `Value` | Primitive value (int, string, bool, etc.) |
| `Enum` | Enumeration type |
| `Pointer` | Pointer/smart pointer |
| `Reference` | Reference wrapper, optional |
| `Custom_Serialize` | Custom serialization handler |
| `Variant` | std::variant |

---

## Macro Reference

### Type Declaration Macros

| Macro | Usage |
|-------|-------|
| `ThorsAnvil_MakeTrait(Type, members...)` | Declare a struct/class as serializable |
| `ThorsAnvil_ExpandTrait(Parent, Type, members...)` | Declare a derived class (includes parent fields) |
| `ThorsAnvil_Template_MakeTrait(N, Type, members...)` | For template types with `N` template parameters |
| `ThorsAnvil_Template_ExpandTrait(N, Parent, Type, members...)` | Template derived class |
| `ThorsAnvil_MakeEnum(EnumType, values...)` | *Deprecated*. Explicit enum value mapping |
| `ThorsAnvil_MakeTraitCustomSerialize(Type, Serializer)` | Register custom serializer for a type |
| `ThorsAnvil_PointerAllocator(Type, Allocator)` | Custom pointer allocation for `Type*` |

### Field Override Macros

| Macro | Usage |
|-------|-------|
| `ThorsAnvil_MakeOverride(Type, {"from","to"}...)` | Rename fields in serialized output |
| `ThorsAnvil_Template_MakeOverride(N, Type, {"from","to"}...)` | Same for template types |

### Polymorphic Macros (inside class definition)

| Macro | Usage |
|-------|-------|
| `ThorsAnvil_PolyMorphicSerializer(Type)` | Add to base class (`public` section) |
| `ThorsAnvil_PolyMorphicSerializerWithOverride(Type)` | Add to derived classes |
| `ThorsAnvil_PolyMorphicSerializerWithName(Type, Name)` | Base class with custom serialized name |
| `ThorsAnvil_RegisterPolyMorphicTypeNamed(Type, Name)` | Register a custom name for a polymorphic type |
| `ThorsAnvil_TypeFieldName(Name)` | Override the type discriminator field name per-class |

### Filter Macros (Deprecated)

| Macro | Usage |
|-------|-------|
| `ThorsAnvil_MakeFilter(Type, filterMember)` | Deprecated -- use `std::optional` instead |

---

## Build Instructions

### Compiling Your Code

```bash
# Basic compilation
g++ -std=c++20 myfile.cpp -lThorSerialize -lThorsLogging

# On Apple Silicon Macs you may need:
g++ -std=c++20 myfile.cpp -I/opt/homebrew/include -L/opt/homebrew/lib \
    -lThorSerialize -lThorsLogging

# For YAML support, also link libyaml:
g++ -std=c++20 myfile.cpp -lThorSerialize -lThorsLogging -lyaml
```

### Visual Studio (Windows)

Add the compiler flag `/Zc:preprocessor` to enable the conforming preprocessor, which is required for the variadic macros used by this library.

---

## Key Design Notes

- **All `ThorsAnvil_MakeTrait` macros must be at global scope** -- not inside a namespace.
- Types in namespaces are supported by using the fully qualified name: `ThorsAnvil_MakeTrait(MyNS::MyType, ...)`.
- The macros generate `Traits<T>` specializations in the `ThorsAnvil::Serialize` namespace.
- Up to 43 members can be listed in a single `ThorsAnvil_MakeTrait()` call.
- Deserialized objects must be default-constructible (the library creates a default object then populates fields).
- By default, unknown fields in the input are silently ignored (`ParseType::Weak`).
