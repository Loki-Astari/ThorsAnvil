#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_PERSON_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_PERSON_H

#include <ThorSerialize/Traits.h>
#include <string>

struct Person
{
    std::string name;
    int         age;
};

struct TwoPeople
{
    Person personOne;
    Person personTwo;
};

ThorsAnvil_MakeTrait(Person, name, age);
ThorsAnvil_MakeTrait(TwoPeople, personOne, personTwo);

#endif
