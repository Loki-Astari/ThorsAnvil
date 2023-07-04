#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using StringTest = SimpleTestDir;

TEST_F(StringTest, WriteMultoLineString)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        ASSERT_TRUE(file);
        file << TwoPeople{Person{"Mult-Line\nString\n1234" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    std::string     line;

    std::ifstream   p1nameFile(simpleTestDir + simpleP1Name);
    std::getline(p1nameFile, line);
    ASSERT_EQ(line, std::string("Mult-Line\000String\0001234", 21));
    ASSERT_TRUE(p1nameFile);
}

TEST_F(StringTest, ReadMultoLineString)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        ASSERT_TRUE(file);
        file << TwoPeople{Person{"Mult-Line\nString\n1234" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }

    TC::IFile<TwoPeople>    file(simpleTestDir);
    TwoPeople               twoPerson;

    file >> twoPerson;
    ASSERT_TRUE(file);

    ASSERT_EQ(twoPerson.personOne.name, "Mult-Line\nString\n1234");
}

