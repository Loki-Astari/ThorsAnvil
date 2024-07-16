# ThorsMonfo

[![Brew package](https://img.shields.io/badge/Brew-package-blueviolet)](https://formulae.brew.sh/formula/thors-serializer)

![ThorStream](img/thorsmongo.jpg)

A modern C++20 library to interact with MongoDB.

This library provides a simple and intuitive library for interacting with a MongoDB.

There are two main parts:

1. [ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer) that automatically converts C++ objects into BSON.
2. [ThorsMongoAPI](https://github.com/Loki-Astari/ThorsMongoAPI) that sends and receives MongoDB wire protocol messages.


# Installing

## Easy: Using Brew

Can be installed via brew on Mac and Linux

    > brew install thors-serializer

* Mac: https://formulae.brew.sh/formula/thors-serializer
* Linux: https://formulae.brew.sh/formula-linux/thors-serializer

## Building Manually

    > git clone git@github.com:Loki-Astari/ThorsMongo.git
    > cd ThorsMongo
    > ./configure
    > make

Note: The `configure` script will tell you about any missing dependencies and how to install them.

## Building Conan

If you have conan installed the conan build processes should work.

    > git clone git@github.com:Loki-Astari/ThorsMongo.git
    > cd ThorsMongo
    > conan build -s compiler.cppstd=20 conanfile.py

## Header Only

To install header only version

    > git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsMongo.git

Some dependencies you will need to install manually for header only builds.

    Magic Enum: https://github.com/Neargye/magic_enum
    libYaml     https://github.com/yaml/libyaml
    libSnappy   https://github.com/google/snappy
    libZ        https://www.zlib.net/

## Building With Visual Studio

To build on windows you will need to add the flag: [`/Zc:preprocessor`](https://learn.microsoft.com/en-us/cpp/build/reference/zc-preprocessor?view=msvc-170). These libraries make heavy use of VAR_ARG macros to generate code for you so require conforming pre-processor. See [Macro Expansion of __VA_ARGS__ Bug in Visual Studio?](https://stackoverflow.com/questions/78605945/macro-expansion-of-va-args-bug-in-visual-studio) for details.


## Contributors

Added the all-contributers bot to generate the table.

