#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using TellTest = SimpleTestDir;

TEST_F(TellTest, tellput)
{
    TC::OFile<TwoPeople>    file(simpleTestDir);
    ASSERT_EQ(file.tellp(), 0);

    file << TwoPeople{Person{"1.1" ,11}, Person{"1.2", 12}};
    ASSERT_EQ(file.tellp(), 1);

    file << TwoPeople{Person{"2.1" ,21}, Person{"2.2", 22}};
    ASSERT_EQ(file.tellp(), 2);

    file << TwoPeople{Person{"3.1" ,31}, Person{"3.2", 32}};
    ASSERT_EQ(file.tellp(), 3);
}

TEST_F(TellTest, tellget)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"1.1" ,11}, Person{"1.2", 12}};
        file << TwoPeople{Person{"2.1" ,21}, Person{"2.2", 22}};
        file << TwoPeople{Person{"3.1" ,31}, Person{"3.2", 32}};
    }
    TwoPeople   people;

    TC::IFile<TwoPeople>    file(simpleTestDir);
    ASSERT_EQ(file.tellg(), 0);

    file >> people;
    ASSERT_EQ(file.tellg(), 1);
    ASSERT_EQ(people.personOne.name, "1.1");

    file >> people;
    ASSERT_EQ(file.tellg(), 2);
    ASSERT_EQ(people.personTwo.name, "2.2");

    file >> people;
    ASSERT_EQ(file.tellg(), 3);
    ASSERT_EQ(people.personOne.age, 31);
}

