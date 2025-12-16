[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/G2G216KZR3)


# ThorsAnvil

![ThorStream](img/thorsanvil.jpg)

A set of modern C++20 libraries for writing interactive Web-Services.

This project provides the following libraries:

1. [Mug](https://github.com/Loki-Astari/Mug)  
   A simple configurable C++ `NissaServer` that dynamically loads shared libraries that install `NisseHTTP` handlers.
1. [Nisse](https://github.com/Loki-Astari/Nisse)  
   + **NisseServer**  
     Provides a server object that handles socket events and non-blocking async IO operations.
   + **NisseHTTP**  
     Provides HTTP request/response objects exposing the body as an async std::iostream. This allows C++ objects to be streamed directly via a REST interface with no serialization code.
1. API Supported:
   + [ThorsSlack](https://github.com/Loki-Astari/Mug/tree/master/src/ThorsSlack)  
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

1. [ThorsIOUtil](https://github.com/Loki-Astari/ThorsIOUtil)
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

## Building Conan

If you have Conan installed, the Conan build processes should work.

    > git clone git@github.com:Loki-Astari/ThorsAnvil.git
    > cd ThorsAnvil
    > conan build -s compiler.cppstd=20 conanfile.py

## Header Only

To install the header-only version

    > git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsAnvil.git

Some dependencies you will need to install manually for header-only builds.

    Magic Enum: https://github.com/Neargye/magic_enum
    libYaml     https://github.com/yaml/libyaml
    libSnappy   https://github.com/google/snappy
    libZ        https://www.zlib.net/
    
Note: The header-only version does not include Mug/Nisse/ThorsSlack.

## Building With Visual Studio

To build on Windows, you will need to add the flag: [`/Zc:preprocessor`](https://learn.microsoft.com/en-us/cpp/build/reference/zc-preprocessor?view=msvc-170). These libraries make heavy use of VAR_ARG macros to generate code for you, so they require a conforming pre-processor. See [Macro Expansion of __VA_ARGS__ Bug in Visual Studio?](https://stackoverflow.com/questions/78605945/macro-expansion-of-va-args-bug-in-visual-studio) for details.

