#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using SeekTest = SimpleTestDir;

TEST_F(SeekTest, seekPutAbsolute)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
        ASSERT_EQ(file.tellp(), 4);

        file.seekp(2);
        file << TwoPeople{Person{"R3.1" ,131}, Person{"R3.2", 132}};
        ASSERT_EQ(file.tellp(), 3);
    }
    {
        TwoPeople       people;
        TC::IFile<TwoPeople>    file(simpleTestDir);

        ASSERT_TRUE(file >> people);
        ASSERT_EQ(people.personOne.name, "N1.1");

        ASSERT_TRUE(file >> people);
        ASSERT_EQ(people.personOne.name, "N2.1");

        ASSERT_TRUE(file >> people);
        ASSERT_EQ(people.personTwo.name, "R3.2");

        ASSERT_TRUE(file >> people);
        ASSERT_FALSE(file >> people);
    }
}

TEST_F(SeekTest, seekPutRelBeginning)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
        ASSERT_EQ(file.tellp(), 4);

        file.seekp(2, std::ios_base::beg);
        file << TwoPeople{Person{"R3.1" ,131}, Person{"R3.2", 132}};
        ASSERT_EQ(file.tellp(), 3);
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N2.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.age, 132);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.age, 41);

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekPutRelEnd)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
        ASSERT_EQ(file.tellp(), 4);

        file.seekp(-3, std::ios_base::end);
        file << TwoPeople{Person{"R2.1" ,121}, Person{"R2.2", 122}};
        ASSERT_EQ(file.tellp(), 2);
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "R2.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.age, 32);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.age, 41);

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekPutRelCurrentNegative)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
        ASSERT_EQ(file.tellp(), 4);

        file.seekp(-4, std::ios_base::cur);
        file << TwoPeople{Person{"R1.1" ,111}, Person{"R1.2", 112}};
        ASSERT_EQ(file.tellp(), 1);
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "R1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N2.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.age, 32);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.age, 41);

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekPutRelCurrentPosative)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
        ASSERT_EQ(file.tellp(), 4);

        file.seekp(1);
        file.seekp(2, std::ios_base::cur);
        file << TwoPeople{Person{"R4.1" ,141}, Person{"R4.2", 142}};
        ASSERT_EQ(file.tellp(), 4);
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N2.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.age, 32);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.age, 141);

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekGetAbsolute)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    file.seekg(1);
    std::cerr << "Seek: " << file.rdstate() << "\n";

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekGetRelBeginning)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    file.seekg(3, std::ios::beg);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    file.seekg(1, std::ios::beg);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekGetRelEnd)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    file.seekg(-3, std::ios::end);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekGetRelCurrentNegative)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    file.seekg(-2, std::ios::cur);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N2.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    file.seekg(-2, std::ios::cur);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N3.2");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    ASSERT_FALSE(file >> people);
}

TEST_F(SeekTest, seekGetRelCurrentPosative)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"N1.1" ,11}, Person{"N1.2", 12}};
        file << TwoPeople{Person{"N2.1" ,21}, Person{"N2.2", 22}};
        file << TwoPeople{Person{"N3.1" ,31}, Person{"N3.2", 32}};
        file << TwoPeople{Person{"N4.1" ,41}, Person{"N4.2", 42}};
    }
    TwoPeople       people;
    TC::IFile<TwoPeople>    file(simpleTestDir);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N1.1");

    file.seekg(1, std::ios::cur);

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personOne.name, "N3.1");

    ASSERT_TRUE(file >> people);
    ASSERT_EQ(people.personTwo.name, "N4.2");

    ASSERT_FALSE(file >> people);
}

