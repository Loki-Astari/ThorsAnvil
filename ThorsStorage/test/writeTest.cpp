#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using WriteTest = SimpleTestDir;

TEST_F(WriteTest, writeTwoPeople)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        ASSERT_TRUE(file);
        file << TwoPeople{Person{"Martin" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    std::string     line;
    int             number;

    std::ifstream   p1nameFile(simpleTestDir + simpleP1Name);
    std::getline(p1nameFile, line);
    ASSERT_EQ(line, "Martin");
    ASSERT_TRUE(p1nameFile);

    std::ifstream   p1ageFile(simpleTestDir + simpleP1Age);
    p1ageFile.read(reinterpret_cast<char*>(&number), sizeof number);
    ASSERT_EQ(number, 20);
    ASSERT_TRUE(p1ageFile);

    std::ifstream   p2nameFile(simpleTestDir + simpleP2Name);
    std::getline(p2nameFile, line);
    ASSERT_EQ(line, "Loki");
    ASSERT_TRUE(p2nameFile);

    std::ifstream   p2ageFile(simpleTestDir + simpleP2Age);
    p2ageFile.read(reinterpret_cast<char*>(&number), sizeof number);
    ASSERT_EQ(number, 22);
    ASSERT_TRUE(p2ageFile);
}

TEST_F(WriteTest, readNormal)
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

TEST_F(WriteTest, badReadDoesNotChangeFile)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        TwoPeople   p1{Person{"Martin", 20}, Person{"Loki",   22}};
        ASSERT_TRUE(
            file << p1
        );

        file.setstate(std::ios::failbit);
        TwoPeople   p2{Person{"BAD",    45}, Person{"STATE",  99}};
        ASSERT_FALSE(
            file << p2
        );
        file.clear();


        TwoPeople   p3{Person{"Again",  28}, Person{"Astari", 29}};
        ASSERT_TRUE(
            file << p3
        );
    }

    TwoPeople               twoP;
    TC::IFile<TwoPeople>    file(simpleTestDir);
    ASSERT_TRUE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Martin");
    ASSERT_EQ(twoP.personTwo.name, "Loki");
    ASSERT_EQ(twoP.personOne.age,  20);
    ASSERT_EQ(twoP.personTwo.age,  22);

    ASSERT_TRUE(file >> twoP);
    ASSERT_EQ(twoP.personOne.name, "Again");
    ASSERT_EQ(twoP.personTwo.name, "Astari");
    ASSERT_EQ(twoP.personOne.age,  28);
    ASSERT_EQ(twoP.personTwo.age,  29);

    ASSERT_FALSE(file >> twoP);
}


