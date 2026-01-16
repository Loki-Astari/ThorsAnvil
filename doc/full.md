![ThorStream](../img/stream.jpg)

## Objective:

    The objective is to make serialization/de-serialization of C++ object to/from
    JSON/YAML/BSON trivial.

    This means:
        1) does not build a JSON/YAML/BSON object. Reads data directly into C++ object.
        2) In normal usage there should be NO need to write any code.
        3) User should not need to understand JSON/YAML/BSON or validate its input.
        4) Should work seamlessly with streams.
        5) Standard containers should automatically work

    I am not concerned about speed.
    Though my trivial test work just fine in terms of speed.
    
    The design was done with the primary goal of communicating with WEB-Servers
    that speak JSON. The main envisioned usage was for mobile devices were many
    small JSON objects are transfered in both directions.

## Marking your class Serializable/Deserializable

* Include the header file: `ThorSerialize/Traits.h`
* Use one of these macros to declare your type as serializable
 *      ThorsAnvil_MakeTrait(DataType, members...)
 *      ThorsAnvil_ExpandTrait(ParentType, DataType, members...)
 *      ThorsAnvil_Template_MakeTrait(TemplateParameterCount, DataType, members...)
 *      ThorsAnvil_Template_ExpandTrait(TemplateParameterCount, ParentType, DataType, members...)
 *
 *      ThorsAnvil_PointerAllocator(DataType, Action)
 *      ThorsAnvil_MakeEnum(EnumType, EnumValues...)
 *
 *      ThorsAnvil_PolyMorphicSerializer(Type)
 *      ThorsAnvil_RegisterPolyMorphicType(Type)
 *
 *      ThorsAnvil_MakeTraitCustomSerialize(Type, SerializableType)

```bash
    DataType:           The name of a class (includeing namespace) of the type
                        you want to be able to serialize at some point.
    ParentType:         The name of a class that has previously been declared
                        using `ThorsAnvil_MakeTrait` or `ThorsAnvil_ExpandTrait`
                        and the parent of `Type`
    TemplateParamCount: If `Type` is a template type the number of parameters it needs
                        so that it is fully generalized.
    members:            A list of member (names) of the class `Type` that need
                        to be serialized.
    EnumValues:         A list of the enum values for the associated enum type.
    Action:             A type that supports to static methods
                            static DataType* alloc()             // Called to default allocate an object. default new
                            static void release(DataType*)       // Called to release object.             default delete
    SerializableType:   A type used to serialize non standard (or complex types).
                        This is specialized for each format type.
                        See the documentation in Traits.h for details.
```

The two macros above build a template specialization of the class `ThorsAnvil::Serialize::Traits<Type>` specific to your class. As a consequence these macros should not be placed inside any namespace blocks.

```C++
    #include "ThorSerialize/Traits.h"

    namespace MyNameSpace
    {
        class MyClass
        {
            public:
                int x;
        };
        // This will fail as it is being used inside the `MyNameSpace` namespace
        ThorsAnvil_MakeTrait(MyClass, x);
    }

    // The correct location to use the macro is here
    ThorsAnvil_MakeTrait(MyNameSpace::MyClass, x);
```

## Private Members
If any members of the class that need to be serialized are private you must define a friendship to allow the `Traits<X>` class to have access to the private members.

```C++
    #include "ThorSerialize/Traits.h"

    namespace MyNameSpace
    {
        class MyClass
        {
            // Allow the traits class to access private members of your class.
            friend class ThorsAnvil::Serialize::Traits<MyClass>;
            double  y;
            public:
                int x;

        };
    }

    ThorsAnvil_MakeTrait(MyNameSpace::MyClass, x, y);
```

## Standard containers
The appropriate declarations for all the standard containers are provided. You simply need to include "ThorSerialize/SerUtil.h" to include these declarations.

```C++
    #include "ThorSerialize/SerUtil.h"
    #include "ThorSerialize/JsonThor.h"
    #include <vector>
    #include <iostream>

    int main()
    {
        using ThorsAnvil::Serialize::jsonExporter;
        using ThorsAnvil::Serialize::PrinterInterface;
        std::vector<int>    data {1,2,3,4,5};
        std::cout << ThorsAnvil::Serialize::jsonExporter(data, PrinterInterface::OutputType::Stream);
    }
```

## Serialization

### JSON
* Include the header file "ThorSerialize/JsonThor.h".
* There are two functions in the namespace `ThorsAnvil::Serialize`.
 * `jsonExporter(<YourObject>, characteristics = Default);`
 * `jsonImporter(<YourObject>);`

Both these methods return an object that simply contains a reference to `YourObject` (no actual serialization happens). When this object is serialized to a stream using `operator<<` or `operator>>` respectively then the code will read/write the appropriate members and serialize/de-serialize them to/from the stream. Because the returned object contains a reference to the object that needs to be serialized; the lifespan should be shorted than `YourObject` to avoid a dangling reference. Therefore it is usually best to just use them directly in the stream operation.

```C++
    std::vector<int>        data{1,2,3,4,5,6};

    std::cout << jsonExporter(data);
    std::cin  >> jsonImporter(data);
```

On export there is a second parameter `characteristics` that allows some simple control on serialization (it basically affects white space to make debugging easier). Values are:
```bash
     Default:     What ever the implementation likes.
                  Currently this means `Config` but I have plans for an
                  application level setting that is checked.
     Stream:      Compressed for over the wire protocol.            ie. No Space.
     Config:      Human readable                                    Potentially config file like.
```

### YAML

The description above is for JSON Serialization/De-serialization. But the exact same description can be used for YAML. Simply replace JSON with YAML and replace JSON with YAML.

The export parameter `characteristics` has slightly different meaning for printing YAML. See the [libyaml documentation](http://libyaml.sourcearchive.com/documentation/0.1.1/group__styles_g1efef592e2e3df6f00432c04ef77d98f.html) for the meaning of these flags.
```bash
     Default:     What ever the implementation likes.
                  Currently this means YAML_BLOCK_MAPPING_STYLE but I have plans for an
                  application level setting that is checked.
     Stream:      YAML_FLOW_MAPPING_STYLE
     Config:      YAML_BLOCK_MAPPING_STYLE
```
### BSON

The description above is for JSON Serialization/De-serialization. But the exact same description can be used for BSON versions. Simply replace JSON with BSON and replace JSON with binary.

The export parameter `characteristics` has no affect on binary.

## Notes on std::map (JSON)

The JSON "Object" is a set of "name"/"value" pairs. But the name part is always a "String". If you use a `std::map<Key, Value>` where the "Key" is a `std::string` then the `std::map<>` will be represented by a JSON "Object". If any other type is used as the "Key" then `std::map<>` will be represented as a JSON "Array" where each member of the array is `std::pair<Key,Value>`.

```C++
    // Example:
    int main()
    {
        std::map<std::string, int>      data1{{"M": 1}};
        std::cout << jsonExporter(data1) << "\n";             // {"M":1}

        std::map<int,int>               data2{{15,2}};
        std::cout << jsonExporter(data2) << "\n";             // [{"first":15, "second":2}]
    }

## Strict Vs Weak Parsing.

By defaul the parser is linient.  
If it finds a "Key" that it does not recognize (or know how to decode) then it will ignore the "Value". This is controlled via the second parameter passed to the parser which defaults to "Weak"

```C++
        using TS = ThorsAnvil::Serializer;
        using PT = TS::ParserInterface::ParseType;

        TS::JasonParser         parser(stream, PT::Strict /* or Weak*/);
        TS::DeSerializeMember   deSer(parser);

        T object;
        deSer.parse(object);

        // -----------
        // Or Short hand

        T object;
        stream >> TS::jsonImporter(object, PT::Strict);
```
## Strict Vs Exact Parsing.

Strict parsing does not allow extra parameters in the JSON input. Exact parsing takes one further step in that all members in the object must be present in the JSON. If not all members are available then an exception is thrown.

```C++
        using TS = ThorsAnvil::Serializer;
        using PT = TS::ParserInterface::ParseType;

        TS::JasonParser         parser(stream, PT::Exact;
        TS::DeSerializeMember   deSer(parser);

        T object;
        deSer.parse(object);

        // -----------
        // Or Short hand

        T object;
        stream >> TS::jsonImporter(object, PT::Exact);    // 
```
## Example-0 [See doc/example0.cpp](example0.cpp)
```C++
    #include <iostream>
    #include <vector>
    #include "ThorSerialize/JsonThor.h"

    int main()
    {
        std::vector<int>    data;
        using ThorsAnvil::Serialize::jsonImporter;
        using ThorsAnvil::Serialize::jsonExporter;

        std::cin >> jsonImporter(data);
        std::cout << jsonExporter(data) << "\n";
    }
```

### Build and run
```bash
    > g++ -std=c++20 example0.cpp -lThorSerialize -lThorsLogging
    > # Note on mac you may need to add -I/opt/homebrew/include -L/opt/homebrew/lib/ on Mac's with M1 chip.
    > echo "[1,2,3,4,5]" | ./a.out
        [ 1, 2, 3, 4, 5]
    > 
```
## Example-1 [See doc/example1.cpp](example1.cpp)

```C++
    #include "ThorSerialize/Traits.h"
    #include "ThorSerialize/JsonThor.h"

    struct Shirt
    {
        int     red;
        int     green;
        int     blue;
    };    
    class TeamMember
    {
        std::string     name;
        int             score;
        int             damage;
        Shirt           team;
        public:
            TeamMember(std::string const& name, int score, int damage, Shirt const& team)
                : name(name)
                , score(score)
                , damage(damage)
                , team(team)
            {}
            // Define the trait as a friend to get accesses to private
            // Members.
            friend class ThorsAnvil::Serialize::Traits<TeamMember>;
    };

    // Declare the traits.
    // Specifying what members need to be serialized.
    ThorsAnvil_MakeTrait(Shirt, red, green, blue);
    ThorsAnvil_MakeTrait(TeamMember, name, score, damage, team);

    int main()
    {
        using ThorsAnvil::Serialize::jsonExporter;

        TeamMember          mark("mark", 10, 5, Shirt{255,0,0});
        // Use the export function to serialize
        std::cout << jsonExporter(mark) << "\n";
    }
```

### Build and run
```bash
    > g++ -std=c++20 expample1.cpp -lThorSerialize -lThorsLogging
    > ./a.out
        { 
            "name": "mark", 
            "score": 10, 
            "damage": 5, 
            "team": 
            { 
                "red": 255, 
                "green": 0, 
                "blue": 0
            }
        }
```
## Example-E [See doc/exampleE.cpp](exampleE.cpp)
```C++
    #include <iostream>
    #include <vector>
    #include "ThorSerialize/JsonThor.h"

    enum class EnumType : int {
        A, B, C
    };

    struct MyStruct {
        EnumType    e;
        std::string s;
    };

    ThorsAnvil_MakeEnum(EnumType, A, B, C);
    ThorsAnvil_MakeTrait(MyStruct, e, s);

    int main()
    {
        using ThorsAnvil::Serialize::jsonImporter;
        using ThorsAnvil::Serialize::jsonExporter;

        MyStruct    val {EnumType::A, "This string"};
        std::cout << jsonExporter(val) << "\n";
    }
```

### Build and run
```bash
    > g++ -std=c++20 example0.cpp -lThorSerialize -lThorsLogging
    > # Note on mac you may need to add -I/opt/homebrew/include -L/opt/homebrew/lib/ on Mac's with M1 chip.
    > ./a.out
    {
        "e": "A",
        "s": "This string"
    }
```
## Example-2: [see doc/example2.cpp](example2.cpp)

```C++
    #include <string>
    #include "ThorSerialize/Traits.h"
    #include "ThorSerialize/SerUtil.h"
    #include "ThorSerialize/JsonThor.h"

    /* A class that you want to serialize. */
    class MyClass
    {   
        int         data1;
        float       data2;
        std::string data3;
        public:
            MyClass(int data1, float data2, std::string const& data3)
                : data1(data1)
                , data2(data2)
                , data3(data3)
            {}

        // This is only required if the members are private.
        friend struct ThorsAnvil::Serialize::Traits<MyClass>;
    };  


    /*  
     * Though there is no code involved, you do need to set up
     * this structure to tell the library what fields need to be serialized.
     * To do this use the macro:  ThorsAnvil_MakeTrait()
     * Specifying your class, and a list of members to serialize.
     */
    ThorsAnvil_MakeTrait(MyClass, data1, data2, data3);
```
    
This allows us to import and export object of the above class really easily.

```C++
    int main()
    {
        using ThorsAnvil::Serialize::jsonExporter;
        using ThorsAnvil::Serialize::PrinterInterface;

        MyClass   data {56, 23.456, "Hi there"};


        // This generates a simple JSON Object (wordy)
        std::cout << "Version 1\n";
        std::cout << jsonExporter(data) << "\n\n\n";

        // This generates a compact JSON 
        std::cout << "Version 2 (Stream)\n";
        std::cout << jsonExporter(data, PrinterInterface::OutputType::Stream) << "\n\n\n";

        // Standard containers work automatically.
        // As long as the type held by the container has had an appropriate
        // Traits declaration.
        std::vector<MyClass>   vec(4, data);
        std::cout << "Vector\n";
        std::cout << jsonExporter(vec) << "\n";
    }
```
    
This generates:

```bash
    > g++ -std=c++20 -o example2 example2.cpp -lThorSerialize -lThorsLogging
    > ./example2
    Version 1
     
        { 
            "data1": 56, 
            "data2": 23.456, 
            "data3": "Hi there"
        }


    Version 2 (Stream)
    {"data1":56,"data2":23.456,"data3":"Hi there"}


    Vector
     [ 
            { 
                "data1": 56, 
                "data2": 23.456, 
                "data3": "Hi there"
            }, 
            { 
                "data1": 56, 
                "data2": 23.456, 
                "data3": "Hi there"
            }, 
            { 
                "data1": 56, 
                "data2": 23.456, 
                "data3": "Hi there"
            }, 
            { 
                "data1": 56, 
                "data2": 23.456, 
                "data3": "Hi there"
            }]
```
## Example-3: [see doc/example3.cpp](example3.cpp)
The library handles polymorphic types via pointers. The only addition the developer needs to do is add the macro `ThorsAnvil_PolyMorphicSerializer()` into the class (as part of the public) 

```C++
    struct Vehicle
    {
        Vehicle(){}
        Vehicle(int speed)
            : speed(speed)
        {}
        virtual ~Vehicle() {}
        int     speed;
        ThorsAnvil_PolyMorphicSerializer(Vehicle);
    };
    struct Car: public Vehicle
    {
        Car(){}
        Car(int speed, std::string const& make)
            : Vehicle(speed)
            , make(make)
        {}
        std::string     make;
        ThorsAnvil_PolyMorphicSerializer(Car);
    };
    struct Bike: public Vehicle
    {
        Bike(){}
        Bike(int speed, int stroke)
            : Vehicle(speed)
            , stroke(stroke)
        {}
        int     stroke;
        ThorsAnvil_PolyMorphicSerializer(Bike);
    };
```

As per normal the class's must also be declared as serializable.
```C++
ThorsAnvil_MakeTrait(Vehicle, speed);
ThorsAnvil_ExpandTrait(Vehicle, Car, make);
ThorsAnvil_ExpandTrait(Vehicle, Bike, stroke);
```

The use cases for serialization/de-serialization are the same:
```C++
int main()
{
    Vehicle*    init = new Bike(15, 2);

    std::stringstream   stream;
    stream << ThorsAnvil::Serialize::jsonExporter(init);
    std::cout << ThorsAnvil::Serialize::jsonExporter(init) << "\n\n";

    Vehicle*    result = nullptr;
    std::cout << ThorsAnvil::Serialize::jsonExporter(result) << "\n\n";
    stream >> ThorsAnvil::Serialize::jsonImporter(result);

    std::cout << ThorsAnvil::Serialize::jsonExporter(result) << "\n\n";
}
```

The one difference from normal serialization is that it adds an extra member to the output class. The key `"__type"` is serialized as the first member of an object. When reading (De-Serializing) a stream the key `"__type"` must be the first member of the object (Otherwise you will get an exception). Notice a `nullptr` is serialized as `null` in JSON.

```bash
    > g++ -std=c++20 -o example3 example3.cpp -lThorSerialize -lThorsLogging
    > ./example3
    {
        "__type": "Bike",
        "speed": 15,
        "stroke": 2
    }

null


    {
        "__type": "Bike",
        "speed": 15,
        "stroke": 2
    }
```

To make this work the `Traits` class for pointers generates a default `alloc()` method that simply calls new on the object (and assumes a default constructor). If you need a custom allocation methods you can specify your own custom one.

```C++
    ThorsAnvil_PointerAllocator(Bike, [](){return new Bike(6,7);});
```

# Install Instructions:
## From [Homebrew](https://brew.sh/)
```bash
brew update
brew install thors-anvil
```

#### What is installed:
* `/usr/local/include/ThorsIOUtil/`
* `/usr/local/include/ThorsLogging/`
* `/usr/local/include/ThorSerialize/`
* `/usr/local/include/ThorsStorage/`
* `/usr/local/include/ThorsCrypto/`
* `/usr/local/include/ThorsSocket/`
* `/usr/local/include/ThorsMongo/`
* `/usr/local/lib/libThorsLogging.so`
* `/usr/local/lib/libThorSerialize.so`
* `/usr/local/lib/libThorsStorage.so`
* `/usr/local/lib/libThorsSocket.so`
* `/usr/local/lib/libThorsMongo.so`
* `/usr/local/share/man/man3/*`

## From [GitHub](https://github.com/Loki-Astari/ThorsSerializer)

The basic script for installing everything is:

```bash
    > git clone https://github.com/Loki-Astari/ThorsSerializer.git
    > cd ThorsSerializer
    > ./configure --disable-binary
    > make
    > sudo make install
```

But installing everything requires a couple of extra libraries and some development tools. You may not need all these tools (try and use brew if you don't).

### YAML
By default it also installs the YAML serialization library. Underneath this uses libyaml this must be install first. If you don't need YAML support then add `--disable-yaml` to the `configure` command above.

### Development

If you want to submit "pull requests" you are going to need vera++. Vera++ is a style checker and is automatically run as part of the build processes. The build will fail if you don't adhere to the style requirements of the project (you must adhere to the style guide for a pull request to be accepted).

If you are simply building for yourself you may not care about the style guide. In this case you can switch it off by adding `--disable-vera` to the `configure` command above.

### Description
By default installation will be in `/usr/local/include` and `/usr/local/lib`. You can override this with the normal auto-tools defaults. Use `./configure --help` to get details.

#### What is installed:
* `/usr/local/include/ThorsIOUtil/`
* `/usr/local/include/ThorsLogging/`
* `/usr/local/include/ThorSerialize/`
* `/usr/local/include/ThorsCrypto/`
* `/usr/local/include/ThorsStorage/`
* `/usr/local/include/ThorsSocket/`
* `/usr/local/include/ThorsMongo/`
* `/usr/local/lib/libThorsLogging.so`
* `/usr/local/lib/libThorSerialize.so`
* `/usr/local/lib/libThorsStorage.so`
* `/usr/local/lib/libThorsSocket.so`
* `/usr/local/lib/libThorsMongo.so`
* `/usr/local/share/man/man3/*`

Note:
libThor<lib>.so is build using `-O3` and thus is fully optimized and debug symbols have been stripped.  
libThor<lib>D.so is build using `-g` and is useful for debugging purposes.


### What is Downloaded
The configuration processes will download the generic makefiles (using git) from [ThorMaker](https://github.com/Loki-Astari/ThorMaker) which in turn will download and build [google's gtest](https://github.com/google/googletest) and [vera++](https://github.com/Loki-Astari/vera-plusplus) library that is used in running the unit tests.

## Requirements
This library uses features from C++14 so you will need a compiler that supports this. The generic makefile also does code coverage tests so your compiler will also need to support a code coverage tool that has an interface similar to `gcov`.


