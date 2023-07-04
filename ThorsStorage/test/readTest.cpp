#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using ReadTest = SimpleTestDir;

TEST_F(ReadTest, readTwoPeople)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"Martin" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    // Just construct and destruct
    // Make sure it does not throw.
    TwoPeople               people;
    TC::File<TwoPeople>     file(simpleTestDir);
    ASSERT_TRUE(file);

    file >> people;
    ASSERT_TRUE(file);

    ASSERT_EQ(people.personOne.name, "Martin");
    ASSERT_EQ(people.personOne.age,  20);
    ASSERT_EQ(people.personTwo.name, "Loki");
    ASSERT_EQ(people.personTwo.age,  22);

    TwoPeople               extra;
    file >> extra;
    ASSERT_FALSE(file);
}

TEST_F(ReadTest, readNormal)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"Martin", 20}, Person{"Loki",   22}};
        ASSERT_TRUE(file);
    }

    TwoPeople               twoP;
    TC::IFile<TwoPeople>    file(simpleTestDir);
    ASSERT_TRUE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Martin");
    ASSERT_EQ(twoP.personTwo.name, "Loki");
    ASSERT_EQ(twoP.personOne.age,  20);
    ASSERT_EQ(twoP.personTwo.age,  22);

    ASSERT_FALSE(file >> twoP);
}

TEST_F(ReadTest, badReadDoesNotChangeFile)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"Martin", 20}, Person{"Loki",   22}};
        file << TwoPeople{Person{"Again",  28}, Person{"Astari", 29}};
        ASSERT_TRUE(file);
    }

    TwoPeople               twoP;
    TC::IFile<TwoPeople>    file(simpleTestDir);
    ASSERT_TRUE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Martin");
    ASSERT_EQ(twoP.personTwo.name, "Loki");
    ASSERT_EQ(twoP.personOne.age,  20);
    ASSERT_EQ(twoP.personTwo.age,  22);

    file.setstate(std::ios::failbit);
    ASSERT_FALSE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Martin");
    ASSERT_EQ(twoP.personTwo.name, "Loki");
    ASSERT_EQ(twoP.personOne.age,  20);
    ASSERT_EQ(twoP.personTwo.age,  22);

    file.clear();
    ASSERT_TRUE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Again");
    ASSERT_EQ(twoP.personTwo.name, "Astari");
    ASSERT_EQ(twoP.personOne.age,  28);
    ASSERT_EQ(twoP.personTwo.age,  29);

    ASSERT_FALSE(file >> twoP);
}

