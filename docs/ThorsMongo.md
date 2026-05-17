[Home](index.html) | [Internal Documentation](internal/ThorsMongo.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseBolt](NisseBolt.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsMongo](ThorsMongo.html) · [ThorsLogging](ThorsLogging.html)

---

# ThorsMongo API

Type-safe MongoDB client for C++20. ThorsMongo builds MongoDB commands as BSON using ThorsSerializer, sends them over a `ThorsSocket` stream, and exposes results as normal C++ objects and ranges.

**Namespace:** `ThorsAnvil::DB::Mongo`

---

## Quick Start

### Makefile

```Makefile
CXXFLAGS    = -std=c++20
LDLIBS      = -lThorsLogging -lThorSerialize -lThorsSocket -lThorsMongo

all:        ThorsMongoApp
```

### ThorsMongoApp.cpp

```cpp
#include "ThorsMongo/ThorsMongo.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class Address
{
    friend class ThorsAnvil::Serialize::Traits<Address>;
    std::string city;
    std::string country;
public:
    Address(std::string c, std::string co)
        : city(std::move(c))
        , country(std::move(co))
    {}
};

class Person
{
    friend class ThorsAnvil::Serialize::Traits<Person>;
    std::string                 name;
    std::uint32_t               age;
    std::vector<std::string>    allergies;
    Address                     address;
public:
    Person(std::string n, std::uint32_t a, Address ad, std::vector<std::string> al = {})
        : name(std::move(n))
        , age(a)
        , allergies(std::move(al))
        , address(std::move(ad))
    {}
};

ThorsAnvil_MakeTrait(Address, city, country);
ThorsAnvil_MakeTrait(Person, name, age, allergies, address);

// Tell ThorsMongo which fields can be used in filters/updates (global scope only)
ThorsMongo_CreateFieldAccess(Person, name);
ThorsMongo_CreateFieldAccess(Person, age);
ThorsMongo_CreateFieldAccess(Person, address, country);

// Build reusable types for filters and updates
using FindByNameEq      = ThorsMongo_FilterFromAccess(Eq,  Person, name);
using FindByAgeGt       = ThorsMongo_FilterFromAccess(Gt,  Person, age);
using IncAge            = ThorsMongo_UpdateFromAccess(Inc, Person, age);
using SetCountry        = ThorsMongo_UpdateFromAccess(Set, Person, address, country);

int main()
{
    using ThorsAnvil::DB::Mongo::ThorsMongo;
    using ThorsAnvil::DB::Mongo::Query;

    ThorsMongo mongo({ "localhost", 27017 }, { "DbUser", "DbPassword", "DB" });
    auto people = mongo["DB"]["People"];

    // Insert
    auto ins = people.insert({ Person{"Tom", 42, Address{"London", "UK"}},
                               Person{"Sam", 27, Address{"Boston", "US"}} });
    if (!ins) {
        std::cerr << "Insert failed: " << ins << "\n";
        return 1;
    }

    // Query: returns a C++ range. Iteration transparently fetches additional batches.
    auto range = people.find<Person>(FindByAgeGt{30});
    if (!range) {
        std::cerr << "Find failed: " << range << "\n";
        return 1;
    }
    for (auto const& p : range) {
        std::cout << ThorsAnvil::Serialize::jsonExporter(p) << "\n";
    }

    // Update one document (server-side)
    auto upd = people.findAndUpdateOne<Person>(FindByNameEq{"Tom"}, IncAge{2});
    if (!upd) {
        std::cerr << "Update failed: " << upd << "\n";
        return 1;
    }

    // Update nested field
    people.findAndUpdateOne<Person>(FindByNameEq{"Sam"}, SetCountry{"USA"});

    // Remove (all matches)
    people.remove(Query<FindByNameEq>{"Tom"});
}
```

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorsMongo/ThorsMongo.h` | Everything you need for connecting and issuing commands |

---

## Core Concepts

### Connection → DB → Collection

- **`ThorsMongo`**: owns the TCP connection and authentication state.
- **`DB`**: lightweight handle for a database name. Construct directly or via `mongo["dbName"]`.
- **`Collection`**: lightweight handle for a collection. Access via `mongo["db"]["collection"]`.

All of these handles are cheap to copy; they refer back to the same underlying connection.

### Results and error handling

Most operations return a result object that supports:

- **`operator bool()`**: `true` means the command succeeded.
- **`operator<<`**: streams a human-readable error message (from the server reply).

This pattern is used for normal result objects (insert/update/remove/etc.) and for range results (find/listCollections/listDatabases).

---

## Connecting

```cpp
using ThorsAnvil::DB::Mongo::ThorsMongo;

ThorsMongo mongo({ "localhost", 27017 }, { "user", "password", "authDB" });
```

Notes:

- **Authentication**: the library currently focuses on SCRAM-SHA-256 (see `third/ThorsMongo/Documentation/DB.md` in the upstream library docs for details).
- **Compression**: the constructor accepts a `Compression` bitmask (e.g. `Compression::Snappy | Compression::ZStd`). The client and server negotiate a mutually supported choice during the handshake.

---

## Data Model (Serialization)

ThorsMongo sends/receives BSON by serializing your C++ types using ThorsSerializer traits.

Minimum requirements for your data types:

- Mark your types serializable using `ThorsAnvil_MakeTrait(...)` (or the template variants).
- Deserialization expects your type to be default-constructible (as with ThorsSerializer generally).

See `docs/ThorsSerialize.md` for the full ThorsSerializer guide.

---

## Insert

`Collection::insert()` accepts either a `std::vector<T>` or a `std::tuple<T...>`.

```cpp
auto result = mongo["DB"]["People"].insert({ person1, person2 });
if (!result) {
    std::cerr << "Insert failed: " << result << "\n";
}
```

---

## Find (cursor-backed range)

`Collection::find<T>()` returns a `FindRange<T>`, which is a C++ range backed by a MongoDB cursor.

Key behavior:

- Iteration may trigger additional network calls (`getMore`) as the cursor advances.
- The cursor is automatically cleaned up (killCursor) when the range’s internal state is destroyed.

```cpp
auto range = mongo["DB"]["People"].find<Person>(FindByAgeGt{30});
if (range) {
    for (auto const& p : range) {
        // use p
    }
}
```

---

## Filters (type-safe query documents)

MongoDB “filters” are BSON documents like:

```json
{"age": {"$gt": 30}}
```

ThorsMongo’s approach is:

- generate a **field access type** that points at a C++ member (including nested members),
- combine that with a **query operator** (`Eq`, `Gt`, `In`, `And`, …),
- pass the resulting object to `find()`, `remove()`, or `findAndModify*()`.

### Field access generation (recommended)

Define the field access once (global scope):

```cpp
ThorsMongo_CreateFieldAccess(Person, age);
ThorsMongo_CreateFieldAccess(Person, address, country);
```

Then define reusable filter types:

```cpp
using FindByAgeGt        = ThorsMongo_FilterFromAccess(Gt, Person, age);
using FindByCountryEq    = ThorsMongo_FilterFromAccess(Eq, Person, address, country);
```

Use them:

```cpp
auto r1 = mongo["DB"]["People"].find<Person>(FindByAgeGt{30});
auto r2 = mongo["DB"]["People"].find<Person>(FindByCountryEq{"UK"});
```

### Combining filters

If you need logical composition, use `ThorsAnvil::DB::Mongo::QueryOp::{And,Or,Nor,Not}`.
For example:

```cpp
using ThorsAnvil::DB::Mongo::QueryOp::And;
using FindOlderInUK = And<FindByAgeGt, FindByCountryEq>;

auto range = mongo["DB"]["People"].find<Person>(FindOlderInUK{30, "UK"});
```

---

## Updates (type-safe update documents)

MongoDB updates are BSON documents like:

```json
{"$inc": {"age": 2}}
```

ThorsMongo builds update documents in the same “field access + operator” style:

```cpp
ThorsMongo_CreateFieldAccess(Person, age);
using IncAge = ThorsMongo_UpdateFromAccess(Inc, Person, age);

mongo["DB"]["People"].findAndUpdateOne<Person>(FindByNameEq{"Tom"}, IncAge{2});
```

### Multiple update operations at once

`findAndUpdateOne()` also accepts a tuple of update expressions:

```cpp
using SetCountry = ThorsMongo_UpdateFromAccess(Set, Person, address, country);
auto updates = std::tuple{ IncAge{2}, SetCountry{"USA"} };

mongo["DB"]["People"].findAndUpdateOne<Person>(FindByNameEq{"Tom"}, updates);
```

---

## Count and Distinct

`Collection` also exposes:

- `countDocuments(...)`
- `distinct(key, ...)`

These follow the same filter pattern and the same `if (result) { ... } else { ... }` error style.

---

## Build Notes

When linking manually you typically need (exact set depends on your build):

```bash
g++ -std=c++20 app.cpp \
  -lThorSerialize -lThorsLogging -lThorsSocket -lThorsMongo
```

---

## Further Reading

- [ThorsMongo internals](internal/ThorsMongo.html)
- [ThorsSerializer](ThorsSerialize.html) (BSON/JSON/YAML traits and config)

