[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/G2G216KZR3)


# ThorsAnvil

![ThorsAnvil](img/thorsanvil.jpg)

A set of modern C++20 libraries for writing interactive Web-Services.

**Online documentation:** https://loki-astari.github.io/ThorsAnvil/

This project provides the following libraries:

1. [NisseBolt](https://github.com/Loki-Astari/NisseBolt)  
   A C++ library for building Slack bots, modeled after the [Slack Bolt for Python](https://slack.dev/bolt-python/) API. Register handlers for messages, commands, actions, and views with the same mental model, but in modern C++20 with automatic serialization and async IO.
1. [Mug](https://github.com/Loki-Astari/Mug)  
   A simple configurable C++ `NissaServer` that dynamically loads shared libraries that install `NisseHTTP` handlers.
1. [Nisse](https://github.com/Loki-Astari/Nisse)  
   + **NisseServer**  
     Provides a server object that handles socket events and non-blocking async IO operations.
   + **NisseHTTP**  
     Provides HTTP request/response objects exposing the body as an async std::iostream. This allows C++ objects to be streamed directly via a REST interface with no serialization code.
1. API Supported:
   + [ThorsSlack](https://github.com/Loki-Astari/NisseBolt/tree/master/src/ThorsSlack)  
     Type-safe API to send REST messages to/from Slack.  
     Supports REST Slack API and Slack webhooks via NissaHTTP.  
     Use C++ objects, no serialization code required.
   + [ThorsMongo](https://github.com/Loki-Astari/ThorsMongo)  
     Type-safe interface for inserting/updating/finding objects in a collection.  
     Sends and receives MongoDB wire protocol messages.  
     Directly send C++ objects to a Mongo collection with no serialization code.
1. [ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer)  
   Automatically converts C++ objects into **JSON** / **BSON** / **YAML**

1. [ThorsSocket](https://github.com/Loki-Astari/ThorsSocket)  
   Async IO library for **Files**/**Pipes**/**Sockets**/**Secure Sockets** that exposes all of them via std::iostream interface.

1. [ThorsCrypto](https://github.com/Loki-Astari/ThorsCrypto)  
   C++ wrapper around platform-specific libraries to support **base64 Encoding**, **CRC Checksum**, **Hashing**, **HMAC**, **SCRAM**, **MD5**, **SHA-1**, **SHA-256**.

1. [ThorsLogging](https://github.com/Loki-Astari/ThorsLogging)


The main goal of these projects is to remove the need to write boilerplate code. Using a declarative style, an engineer can define the C++ classes and members that need to be serialized.

# Write-Ups
Detailed explanations of these projects can be found at:

[LokiAstari.com](https://lokiastari.com/series)

# Installing

## Easy: Using Brew

Can be installed via brew on Mac and Linux

    > brew install thors-anvil

* Mac: https://formulae.brew.sh/formula/thors-anvil
* Linux: https://formulae.brew.sh/formula-linux/thors-anvil

## Building Manually

    > git clone git@github.com:Loki-Astari/ThorsAnvil.git
    > cd ThorsAnvil
    > ./configure
    > make

Note: The `configure` script will tell you about any missing dependencies and how to install them.

## Installing via Conan

ThorsAnvil is available on [ConanCenter](https://conan.io/center/recipes/thors-anvil):

    > conan install --requires="thors-anvil/10.2.2" -s compiler.cppstd=20

## Header Only

To install the header-only version

    > git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsAnvil.git

Some dependencies you will need to install manually for header-only builds.

    Magic Enum: https://github.com/Neargye/magic_enum
    libYaml     https://github.com/yaml/libyaml
    libSnappy   https://github.com/google/snappy
    libZ        https://www.zlib.net/
    
Note: The header-only version does not include Mug or NisseBolt

# Using ThorsAnvil In Your Project (CMake)

Installing ThorsAnvil (via `make install` or `brew install thors-anvil`) also installs a CMake package config file to `<prefix>/lib/cmake/ThorsAnvil/`. This allows any CMake-based project to consume the libraries via `find_package`.

## Quick Start

In your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyApp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ThorsAnvil REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ThorsAnvil::ThorSerialize)
```

If ThorsAnvil is installed to a non-standard prefix, point CMake at it:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/install/prefix
```

## Available Imported Targets

Each target brings in the appropriate include directories and transitive dependencies automatically.

| Target                          | Purpose                                                |
|---------------------------------|--------------------------------------------------------|
| `ThorsAnvil::ThorsLogging`      | Leveled logging macros                                 |
| `ThorsAnvil::ThorSerialize`     | JSON / YAML / BSON serialization                       |
| `ThorsAnvil::ThorsCrypto`       | Base64, CRC, MD5, SHA, HMAC, SCRAM (header-only)       |
| `ThorsAnvil::ThorsSocket`       | Async IO (files / pipes / sockets / TLS) as iostream   |
| `ThorsAnvil::ThorsStorage`      | Columnar file storage                                  |
| `ThorsAnvil::ThorsMongo`        | Type-safe MongoDB wire protocol client                 |
| `ThorsAnvil::Nisse`             | Coroutine-based async HTTP server                      |
| `ThorsAnvil::NisseBolt`         | Slack bot framework (Bolt API for C++)                 |

## Example

```cmake
find_package(ThorsAnvil REQUIRED)

add_executable(webserver main.cpp)
target_link_libraries(webserver
    PRIVATE
        ThorsAnvil::Nisse            # async HTTP server
        ThorsAnvil::ThorSerialize    # JSON request/response bodies
        ThorsAnvil::ThorsMongo       # MongoDB persistence
)
```

External dependencies (`OpenSSL`, `libevent`, `boost`, `libyaml`, `snappy`, `zlib`) must also be available on the system — the package config pulls them in automatically where required.

## Building With Visual Studio

To build on Windows, you will need to add the flag: [`/Zc:preprocessor`](https://learn.microsoft.com/en-us/cpp/build/reference/zc-preprocessor?view=msvc-170). These libraries make heavy use of VAR_ARG macros to generate code for you, so they require a conforming pre-processor. See [Macro Expansion of __VA_ARGS__ Bug in Visual Studio?](https://stackoverflow.com/questions/78605945/macro-expansion-of-va-args-bug-in-visual-studio) for details.

