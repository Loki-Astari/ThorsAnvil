[Home](index.html) | [Internal Documentation](internal/ThorsSerialize.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsSerializer API

Declarative C++ serialization for JSON, YAML, and BSON. Instead of manually building a DOM or writing conversion code, declare which members of your C++ types should be serialized and the library generates everything at compile time.

**Namespace:** `ThorsAnvil::Serialize`

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
echo "[1,2,3,4,5]" | ./a.out
# Output: [1, 2, 3, 4, 5]
```

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorSerialize/Traits.h` | Core macros for declaring serializable types |
| `ThorSerialize/SerUtil.h` | Support for standard library containers |
| `ThorSerialize/JsonThor.h` | JSON exporter/importer |
| `ThorSerialize/YamlThor.h` | YAML exporter/importer |
| `ThorSerialize/BsonThor.h` | BSON exporter/importer |

---

## Declaring Serializable Types

### Simple Structs

Use `ThorsAnvil_MakeTrait()` at **global scope** (outside any namespace):

```cpp
struct Color
{
    int red;
    int green;
    int blue;
};

ThorsAnvil_MakeTrait(Color, red, green, blue);
```

If `Color` is in a namespace:

```cpp
ThorsAnvil_MakeTrait(Graphics::Color, red, green, blue);
```

### Classes with Private Members

Declare `Traits<T>` as a friend:

```cpp
class User
{
    friend class ThorsAnvil::Serialize::Traits<User>;
    std::string name;
    int         age;
public:
    User() : name(""), age(0) {}
    User(std::string n, int a) : name(std::move(n)), age(a) {}
};

ThorsAnvil_MakeTrait(User, name, age);
```

### Inheritance

Use `ThorsAnvil_ExpandTrait()` to include parent fields automatically:

```cpp
struct Base     { int value1; int value2; };
struct Derived : public Base { int value3; int value4; };

ThorsAnvil_MakeTrait(Base, value1, value2);
ThorsAnvil_ExpandTrait(Base, Derived, value3, value4);
// Serializing Derived includes value1, value2, value3, value4
```

### Template Types

```cpp
template<typename T>
struct Wrapper { T data; };

ThorsAnvil_Template_MakeTrait(1, Wrapper, data);  // 1 = number of template params
```

---

## Serialization (Writing)

```cpp
#include "ThorSerialize/JsonThor.h"
using namespace ThorsAnvil::Serialize;

MyType obj;

// Write to stdout
std::cout << jsonExporter(obj);

// Write to a string
std::string output;
output << jsonExporter(obj);

// YAML / BSON
std::cout << yamlExporter(obj);
std::cout << bsonExporter(obj);
```

---

## Deserialization (Reading)

```cpp
using namespace ThorsAnvil::Serialize;

MyType obj;

// Read from stdin
std::cin >> jsonImporter(obj);

// Read from a string
std::string input = R"({"name": "Alice", "age": 30})";
input >> jsonImporter(obj);

// Build in one expression
auto user = jsonBuilder<User>(std::string{R"({"name":"Alice","age":30})"});
```

---

## Configuration

### PrinterConfig (Export)

```cpp
PrinterConfig config;
config.setOutputType(OutputType::Config)      // pretty-printed
      .setCatchExceptions(true);

std::cout << jsonExporter(myObj, config);
```

| Method | Default | Description |
|--------|---------|-------------|
| `setOutputType()` | `Default` | `Stream` = compressed, `Config` = pretty-printed |
| `setPolymorphicMarker()` | `"__type"` | Key name for polymorphic type identification |
| `setCatchExceptions()` | `true` | Catch `std::exception` and set failbit instead of propagating |
| `setTabSize()` | 4 (JSON) / 2 (YAML) | Indent size |

### ParserConfig (Import)

```cpp
ParserConfig config;
config.setParseStrictness(ParseType::Strict);

std::cin >> jsonImporter(myObj, config);
```

| Method | Default | Description |
|--------|---------|-------------|
| `setParseStrictness()` | `Weak` | `Weak`: ignore unknown fields. `Strict`: unknown fields throw. `Exact`: unknown AND missing fields throw. |
| `setPolymorphicMarker()` | `"__type"` | Key name for polymorphic type identification |
| `setValidateNoTrailingData()` | `false` | Fail if non-whitespace data follows the parsed object |

---

## Supported Standard Library Types

Include `ThorSerialize/SerUtil.h` to get automatic support:

| Type | Serialized As |
|------|---------------|
| `std::vector<T>`, `std::list<T>`, `std::deque<T>` | array |
| `std::set<T>`, `std::unordered_set<T>` | array |
| `std::array<T, N>`, `std::tuple<Args...>` | array |
| `std::map<std::string, V>` | object |
| `std::map<K, V>` (non-string keys) | array of pairs |
| `std::pair<F, S>` | object with `"first"`, `"second"` |
| `std::optional<T>` | value if present, **omitted** if empty |
| `std::unique_ptr<T>` | value if non-null, `null` otherwise |
| `std::shared_ptr<T>` | deduplicated serialization |
| `std::variant<Args...>` | serialized as the active alternative |

Primitive types (`int`, `double`, `bool`, `std::string`, etc.) are handled automatically.

---

## Enum Handling

Enums are serialized as string names automatically via [magic_enum](https://github.com/Neargye/magic_enum):

```cpp
enum class Color { Red, Green, Blue };
struct Shirt { Color color; };
ThorsAnvil_MakeTrait(Shirt, color);
// Serializes as: {"color": "Red"}
```

---

## Field Name Overrides

Map C++ field names to different serialized names:

```cpp
struct MongoQuery { std::string database; std::string collection; };

ThorsAnvil_MakeOverride(MongoQuery, {"database", "$db"}, {"collection", "$coll"});
ThorsAnvil_MakeTrait(MongoQuery, database, collection);
// Produces: {"$db": "mydb", "$coll": "users"}
```

---

## Optional Fields

Fields declared as `std::optional<T>` are omitted from output when empty:

```cpp
struct UserProfile
{
    std::string             name;
    std::optional<int>      age;
    std::optional<std::string> email;
};
ThorsAnvil_MakeTrait(UserProfile, name, age, email);

UserProfile u{"Alice", std::nullopt, "alice@example.com"};
std::cout << jsonExporter(u);
// Output: {"name": "Alice", "email": "alice@example.com"}
```

---

## Polymorphic Serialization

Serialize base-class pointers with automatic type discrimination:

```cpp
struct Transport
{
    int wheelCount;
    virtual ~Transport() = default;
    ThorsAnvil_PolyMorphicSerializer(Transport);
};

struct Car : public Transport
{
    std::string engineType;
    ThorsAnvil_PolyMorphicSerializerWithOverride(Car);
};

ThorsAnvil_MakeTrait(Transport, wheelCount);
ThorsAnvil_ExpandTrait(Transport, Car, engineType);

// Serialized JSON includes "__type" discriminator:
// {"__type": "Car", "wheelCount": 4, "engineType": "V8"}
```

---

## Custom Serialization

For types that need non-standard serialization:

```cpp
struct MyTypeSerializer
{
    static void writeCustom(PrinterInterface& printer, MyType const& object);
    static void readCustom(ParserInterface& parser, MyType& object);
};

ThorsAnvil_MakeTraitCustomSerialize(MyType, MyTypeSerializer);
```

---

## Complete Example

```cpp
#include <iostream>
#include <sstream>
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"

struct Shirt { int red; int green; int blue; };

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

---

## Macro Reference

| Macro | Usage |
|-------|-------|
| `ThorsAnvil_MakeTrait(Type, members...)` | Declare a serializable type |
| `ThorsAnvil_ExpandTrait(Parent, Type, members...)` | Declare a derived serializable type |
| `ThorsAnvil_Template_MakeTrait(N, Type, members...)` | Serializable template type |
| `ThorsAnvil_Template_ExpandTrait(N, Parent, Type, members...)` | Derived template type |
| `ThorsAnvil_MakeOverride(Type, {"from","to"}...)` | Rename fields in output |
| `ThorsAnvil_MakeTraitCustomSerialize(Type, Serializer)` | Custom serialization |
| `ThorsAnvil_PolyMorphicSerializer(Type)` | Polymorphic base class |
| `ThorsAnvil_PolyMorphicSerializerWithOverride(Type)` | Polymorphic derived class |
| `ThorsAnvil_PointerAllocator(Type, Allocator)` | Custom pointer allocation |

