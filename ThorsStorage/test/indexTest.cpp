#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using IndexTest = SimpleTestDir;

TEST_F(IndexTest, indexFileCheck)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
        file << TwoPeople{Person{"Martin" ,20}, Person{"Loki", 22}};
        ASSERT_TRUE(file);
    }
    std::size_t     index;

    std::fstream    index1((simpleTestDir + simpleP1NameI).c_str());
    ASSERT_TRUE(index1);

    index1.read(reinterpret_cast<char*>(&index), sizeof index);
    ASSERT_TRUE(index1);

    ASSERT_EQ(index, 7);    // Martin + '\n'


    std::fstream    index2((simpleTestDir + simpleP1NameI).c_str());
    ASSERT_TRUE(index2);

    index2.read(reinterpret_cast<char*>(&index), sizeof index);
    ASSERT_TRUE(index2);

    ASSERT_EQ(index, 7);    // Loki + '\n'
}
