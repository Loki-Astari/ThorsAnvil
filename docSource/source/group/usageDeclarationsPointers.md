---
layout: plain
generate: false
---
````C++
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/SerUtil.h"
#include "TeamMember.h"


using ThorsAnvil::Serialize::jsonExport;
using ThorsAnvil::Serialize::jsonImport;

int main()
{
    std::unique_ptr<TeamMember>     data;
    std::cout << jsonExport(data) << "\n";
    std::cout << jsonExport(data.get()) << "\n";

    data.reset(new TeamMember{"Tim", 33, 2, Shirt{12, 13, 14}});
    std::cout << jsonExport(data) << "\n";
    std::cout << jsonExport(data.get()) << "\n";
}
...
 null
 null

	{
		"name": "Tim",
		"score": 33,
		"damage": 2,
		"team":
		{
			"red": 12,
			"green": 13,
			"blue": 14
		}
	}

	{
		"name": "Tim",
		"score": 33,
		"damage": 2,
		"team":
		{
			"red": 12,
			"green": 13,
			"blue": 14
		}
	}
````
No extra work needs to be done to serialize pointers. If the pointer is `nullptr` it will serialize to the `null` type of the serialization format. If the pointer is not null then it will serialize as an object.

#### std::unique_ptr
The `std::unique_ptr<>` behaves just like a pointer during serialization.

#### std::shared_ptr
The `std::shared_ptr` is correctly de-dupped. i.e. If you have two (or more) shared pointers pointing at the same object then the object is only serialized once (the second time we serialize an id). When de-serializing shared_ptr the reference id is used to reconnect the objects so that the shared pointers now continue to use a shared object.

#### Polymorphic Objects
````C++
#include "ThorSerialize/JsonThor.h"
#include "TeamMember.h"

using ThorsAnvil::Serialize::jsonExport;
using ThorsAnvil::Serialize::jsonImport;

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
ThorsAnvil_MakeTrait(Vehicle, speed);
ThorsAnvil_ExpandTrait(Vehicle, Car, make);
ThorsAnvil_ExpandTrait(Vehicle, Bike, stroke);

int main()
{
    Vehicle* v = new Bike(12,3);
    std::cout << jsonExport(v) << "\n";
}
...
	{
		"__type": "Bike",
		"speed": 12,
		"stroke": 3
	}

````
Polymorphic objects are supported. **BUT** require an intrusive change in the type. To mark objects as polymorphic you need to add the macro `ThorsAnvil_PolyMorphicSerializer()`. This macro adds a couple of virtual methods to your class (but no data members). Additonally the resulting JSON has an extra field `__ type` that contains the name of the type. This allows the serialization library to dynamically create the correct type at runtime.


