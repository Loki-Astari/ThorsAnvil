# ThorsMongo

[![Brew package](https://img.shields.io/badge/Brew-package-blueviolet)](https://formulae.brew.sh/formula/thors-mongo)

![ThorStream](img/thorsmongo.jpg)

A modern C++20 library to interact with MongoDB.

This library provides a simple and intuitive library for interacting with a MongoDB.

There are two main parts:

1. [ThorsSerializer](https://github.com/Loki-Astari/ThorsSerializer) automatically converts C++ objects into BSON (JSON/YAML).
2. [ThorsMongoAPI](https://github.com/Loki-Astari/ThorsMongoAPI) sends and receives MongoDB wire protocol messages.

The main goal of this project is to remove the need to write boilerplate code to save/ restore C++ objects into a MongoDB. Using a declarative style an engineer can define the C++ classes and members that need to be serialized into BSON thus allowing them to be inserted into or retrieved directly to/from a MongoDB.

## Example:

```C++
    #include "ThorsMongo/ThorsMongo.h"
    #include <vector>
    #include <string>

    class Address
    {
        friend class ThorsAnvil::Serialize::Traits<Address>;
        std::string     street;
        std::string     city;
        std::string     country;
        std::string     postCode;
        public:
            // Add your API here
    };
    using Allergies = std::vector<std::string>;
    class Person
    {
        friend class ThorsAnvil::Serialize::Traits<Person>;
        std::string     name;
        std::uint32_t   age;
        Address         address;
        Allergies       alergies;
        public:
            // Add your API here
    };

    // Make the classes serialize able into BSON.
    ThorsAnvil_MakeTrait(Address, street, city, country, postCode);
    ThorsAnvil_MakeTrait(Person, name, age, address, alergies);

    // Define what fields can be used in Search/Update
    ThorsMongo_CreateFieldAccess(Person, name);             // Search/Update a person by name.
    ThorsMongo_CreateFieldAccess(Person, age);              // Search/Update a person by age.
    ThorsMongo_CreateFieldAccess(Person, address, country); // Search/Update a person by country.

    // Define a class that can be used to search for a person by name using 'Eq' (equal)
    using FindEqName = ThorsMongo_FilterFromAccess(Eq, Person, name);
    // Define a class that can be used to search for a person by age age using 'Gt' (Greater than)
    using FindGtAge = ThorsMongo_FilterFromAccess(Gt, Person, age);
    // Define a class that increments age
    using IncAge = ThorsMongo_UpdateFromAccess(Inc, Person, age);
    // Define a class that sets the country.
    using SetCountry = ThorsMongo_UpdateFromAccess(Set, Person, address, country);

    std::vector<Person> readDataFromFile()
    {
        // Read all the people you want to put in the DB
        return {};
    }
    int main()
    {
        using ThorsAnvil::DB::Mongo::ThorsMongo;
        using ThorsAnvil::DB::Mongo::Query;
        std::vector<Person> data = readDataFromFile();  // Write this function to read data from file.

        ThorsMongo  mongo({"localhost", 27017}, {"DbUser", "UserPassword"});
        mongo["DB"]["PeopleCollection"].insert(data);
        mongo["DB"]["PeopleCollection"].remove(Query<FindEqName>{"John"});      // Remove all the people named "John"
        auto find = mongo["DB"]["PeopleCollection"].find<Person>(FindGtAge{51});// Find all the people over 51
        for (auto const& person: find) {
            // Now you a person
        }
        mongo["DB"]["PeopleCollection"].findAndUpdateOne<Person>(FindEqName{"Tom"}, IncAge{2});         // Increment the age of Tom by 2
        mongo["DB"]["PeopleCollection"].findAndUpdateOne<Person>(FindEqName{"Sam"}, SetCountry{"USA"}); // Sam now lives in the USA
    }
```

Builing the above application:

```bash
> export THORS_ROOT=<Location where ThorsMongo Is Installed>
> g++ -std=c++20 Example.cpp -I ${THORS_ROOT}/include -L ${THORS_ROOT}/lib -lThorSerialize -lThorsLogging -lThorsMongo -lThorsSocket
```

# Installing

## Easy: Using Brew

Can be installed via brew on Mac and Linux

    > brew install thors-mongo

* Mac: https://formulae.brew.sh/formula/thors-mongo
* Linux: https://formulae.brew.sh/formula-linux/thors-mongo

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

