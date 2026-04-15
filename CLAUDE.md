# CLAUDE.md

## Project Overview

ThorsAnvil is a C++20 monorepo of libraries for building interactive web services. The core design goal is eliminating serialization boilerplate through declarative, compile-time traits and providing async IO that looks like synchronous code. Components are git submodules under `third/` with source symlinked into `src/`.

GitHub: `Loki-Astari/ThorsAnvil`

## Build System

Custom autotools-based build system (`ThorMaker`) with template Makefiles in `build/tools/`.

```bash
# Initial setup (run once after clone)
./configure

# Build everything
make

# Build without installing binaries
./configure --disable-binary && make

# Install
make install

# Clean
make clean       # normal clean
make veryclean   # deep clean including generated files

# Header-only build
make header-only
```

### Alternative Build Methods
```bash
# Conan
conan build -s compiler.cppstd=20 conanfile.py

# Header-only branch (no compilation needed, excludes Mug)
git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsAnvil.git

# Homebrew
brew install thors-anvil
```

**Windows:** Add `/Zc:preprocessor` to MSVC flags — the VAR_ARG macros require a conforming preprocessor. Build under MSYS2 with `MSYS=winsymlinks:nativestrict`.

### Configure Flags
```bash
--disable-vera                        # Skip static analysis
--disable-colour                      # No colored output
--disable-test-with-mongo-query       # Skip Mongo query tests
--disable-Mongo-Service               # Skip Mongo service tests
--enable-dark-mode                    # Dark terminal theme
--with-magicenum-header-only-root=... # Magic Enum include path
```

### Component Makefile Pattern

Each component has a `Makefile` like:
```makefile
CONFIG_NAME = ThorsSerializer
THORSANVIL_ROOT ?= $(realpath ../../)
COVERAGE_REQUIRED = 70
TARGET = ThorSerialize.lib
NAMESPACE = ThorsAnvil::Serialize
LDLIBS_EXTERN_BUILD += MagicEnum yaml
LINK_LIBS += ThorsLogging
include $(THORSANVIL_ROOT)/build/tools/Makefile
```

**Build target types** (by file extension in TARGET):
- `*.lib` / `*.slib` — shared library (.so/.dylib/.dll)
- `*.a` — static library
- `*.head` — header-only library
- `*.defer` — compile objects only, linked into a later library
- `*.prog` — executable
- `*.test` — test target (not installed)
- `*.dir` — recurse into subdirectory

## Testing

Tests use **GoogleTest** and live in `src/<Component>/test/` directories.

```bash
make test                        # Run all tests
make test-ClassName.*            # Run specific test class
make test-ClassName.Method       # Run specific test method
make testrun.ClassName.*         # Run test without rebuilding
make debugrun.ClassName.*        # Run test in debugger
```

Coverage uses gcov; default minimum is 70% per component (`COVERAGE_REQUIRED`).

## Repository Layout

```
third/               # Git submodules (actual source)
src/                 # Symlinks into third/ for flat build access
build/               # Build infrastructure (ThorMaker submodule)
  tools/             # main.Makefile, Project.Makefile, Platform.Makefile
  include/           # Installed headers
  lib/               # Compiled libraries
  googletest/        # GoogleTest framework
apps/                # Example applications
brew/                # Homebrew packaging
docSource/           # Documentation source (Hugo)
docs/                # Generated documentation
.github/workflows/   # CI/CD (Mac, Linux, Windows, header-only)
```

### Build Order (from `src/Makefile`)
```
ThorsLogging → fast_float → Serialize → ThorsCrypto →
ThorsStorage → ThorsSocket → ThorsMongo → NisseServer → NisseHTTP →
NisseLib → ThorsMug → ThorsSlack
```

## Component Libraries

| Library | Namespace | Purpose |
|---|---|---|
| **ThorsSerializer** | `ThorsAnvil::Serialize` | JSON/YAML/BSON serialization via compile-time traits |
| **ThorsSocket** | `ThorsAnvil::ThorsSocket` | Async IO (files/pipes/sockets/TLS) as `std::iostream` |
| **Nisse** (NisseServer + NisseHTTP + NisseLib) | `ThorsAnvil::Nisse::Server` / `ThorsAnvil::Nisse::HTTP` | Coroutine-based async server framework |
| **Mug** (ThorsMug) | — | Hot-loadable plugin server reading JSON config |
| **ThorsMongo** | `ThorsAnvil::ThorsMongo` | Type-safe MongoDB wire protocol client |
| **ThorsSlack** | — | Type-safe Slack REST/webhook client |
| **ThorsCrypto** | — | Base64, CRC, hashing (MD5/SHA-1/SHA-256), HMAC, PBKDF2, SCRAM |
| **ThorsLogging** | `ThorsAnvil::Logging` | Leveled logging macros (`ThorsLogFatal` through `ThorsLogAll`) |
| **ThorsStorage** | — | Columnar file storage with per-field files and index-based seeking |

All code lives under `namespace ThorsAnvil` with component sub-namespaces.

## Core Pattern: Trait-Based Serialization

Types are made serializable by declaring traits — no runtime overhead, no hand-written serialization code:

```cpp
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"

struct Person { std::string name; int age; };
ThorsAnvil_MakeTrait(Person, name, age);

// Serialize to any format:
std::cout << jsonExporter(person);
std::cout << yamlExporter(person);  // YamlThor.h
std::cout << bsonExporter(person);  // BsonThor.h

// Deserialize:
std::cin >> jsonImporter(person);
```

### Key Macros
```cpp
ThorsAnvil_MakeTrait(Type, member1, member2, ...)            // Declare serializable fields
ThorsAnvil_ExpandTrait(Parent, Derived, member1, ...)        // Inheritance
ThorsAnvil_Template_MakeTrait(ParamCount, Type, ...)         // Template types
ThorsAnvil_MakeEnum(EnumType, Value1, Value2, ...)           // Enum as string
ThorsAnvil_MakeOverride(Type, {From, To}, ...)               // Field name mapping
ThorsAnvil_MakeTraitCustomSerialize(Type, CustomType)        // Custom serialization
ThorsAnvil_PolyMorphicSerializer(Type)                       // Polymorphic pointers
ThorsAnvil_PointerAllocator(Type, AllocationType)            // Custom pointer handling
```

### TraitType Enum
`enum class TraitType { Invalid, Parent, Value, Map, Array, Enum, Pointer, Reference, Variant, Custom_Serialize, AnyBlock }` — dispatches serialization at compile time.

## Async Architecture (Nisse / ThorsSocket)

- **Event-driven** wrapper around `libevent`
- **Coroutine-based** async with `boost::coroutines2` — code reads like synchronous blocking IO
- **Three-yield protocol:** `RestoreRead`, `RestoreWrite`, `Remove`
- **ThorsSocket** exposes files/pipes/TCP/TLS as `std::iostream`

### NisseHTTP Example
```cpp
HTTPHandler handler;
handler.addPath(Method::GET, "/hello/{name}", [](Request& req, Response& res) {
    res.body(Encoding::Chunked) << "Hello " << req.variables()["name"];
    return true;
});
NisseServer server;
server.listen(ServerInfo{8080}, handler);
server.run();
```

#o## Mug Plugin Architecture
Mug reads a JSON config specifying ports and shared-library plugins. Plugins implement `MugPlugin` and export `extern "C" MugPlugin* mugCreateInstance(const char*)`. Mug monitors plugin files and auto-reloads on modification.

## External Dependencies

Configured via `./configure`: OpenSSL, libevent, Boost (≥1.70 for Coroutine2), libYaml, Snappy, ZLib, Magic Enum.

## CI/CD

GitHub Actions (`.github/workflows/build.yml`) runs on push to `master`:
- **macOS** (macos-latest): brew install deps, configure, make, make install
- **Linux** (ubuntu-22.04): apt-get deps, clone magic_enum, configure, make
- **Windows** (windows-latest): MSYS2, MinGW deps, configure, make
- **Header-only**: builds header-only variant
