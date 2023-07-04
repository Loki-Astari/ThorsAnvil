#include "gtest/gtest.h"

#include "file.h"
#include "test/Person.h"
#include "test/TestFileClass.h"

namespace TC=ThorsAnvil::FileSystem::ColumnFormat;

using CloseTest = SimpleTestDir;

TEST_F(CloseTest, closeTwoPeopleFile)
{
    TC::File<TwoPeople>    file(simpleTestDir, std::ios::out);
    ASSERT_TRUE(file);

    file.close();
    ASSERT_FALSE(file);
}

TEST_F(CloseTest, closeTwoPeopleOFile)
{
    TC::OFile<TwoPeople>    file(simpleTestDir);
    ASSERT_TRUE(file);

    file.close();
    ASSERT_FALSE(file);
}

TEST_F(CloseTest, closeTwoPeopleIFile)
{
    {
        TC::OFile<TwoPeople>    file(simpleTestDir);
    }
    TC::IFile<TwoPeople>    file(simpleTestDir);
    ASSERT_TRUE(file);

    file.close();
    ASSERT_FALSE(file);
}

