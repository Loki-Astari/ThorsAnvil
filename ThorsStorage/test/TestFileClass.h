#ifndef THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H
#define THORSANVIL_FILESYSTEM_COLUMNFORMAT_TEST_TESTCLASS_H

#include "gtest/gtest.h"
#include <stdexcept>
#include <fcntl.h>

using namespace std::string_literals;

static std::string const   testDataDir       = "runTestDir"s;

static std::string const   testSecondOpenDir = "runTestDir/second"s;

static std::string const   simpleIndex   = "/$index"s;
static std::string const   simpleP1Dir   = "/personOne"s;
static std::string const   simpleP1Name  = "/personOne/name"s;
static std::string const   simpleP1NameI = "/personOne/name.index"s;
static std::string const   simpleP1Age   = "/personOne/age"s;
static std::string const   simpleP2Dir   = "/personTwo"s;
static std::string const   simpleP2Name  = "/personTwo/name"s;
static std::string const   simpleP2NameI = "/personTwo/name.index"s;
static std::string const   simpleP2Age   = "/personTwo/age"s;

static std::string const   simpleTestDir = "runTestDir/PersonData"s;
static std::string const   lockedTestDir = "runTestDir/lockedDir"s;
static std::string const   lockedFileDir = "runTestDir/openDir"s;

// The fixture for testing class Foo.
class TestFileClass: public ::testing::Test
{
    public:
        TestFileClass()
        {
            int checkPreCondition = access(testDataDir.c_str(), F_OK);
            if (checkPreCondition == 0)
            {
                // Run Time Test Directory exists.
                // A previous test has failed to clean up correctly.
                throw std::runtime_error("Test Directory Detected: It should not be there");
            }
            mkdir(testDataDir.c_str(), 0'777);
        }
        ~TestFileClass()
        {
            remove(testDataDir.c_str());
        }
};

class TwoPeopleTest: public TestFileClass
{
    std::string fileNameBase;
    protected:
        TwoPeopleTest(std::string const& fileNameBase)
            : fileNameBase(fileNameBase)
        {}
        void TearDown() override
        {
            chmod((fileNameBase + simpleP1Name ).c_str(), 0'777);
            chmod((fileNameBase + simpleP1NameI).c_str(), 0'777);
            chmod((fileNameBase + simpleP1Age  ).c_str(), 0'777);
            chmod((fileNameBase + simpleP1Dir  ).c_str(), 0'777);

            chmod((fileNameBase + simpleP2Name ).c_str(), 0'777);
            chmod((fileNameBase + simpleP2NameI).c_str(), 0'777);
            chmod((fileNameBase + simpleP2Age  ).c_str(), 0'777);
            chmod((fileNameBase + simpleP2Dir  ).c_str(), 0'777);

            chmod((fileNameBase + simpleIndex  ).c_str(), 0'777);

            chmod((fileNameBase                ).c_str(), 0'777);

            // --

            remove((fileNameBase + simpleP1Name ).c_str());
            remove((fileNameBase + simpleP1NameI).c_str());
            remove((fileNameBase + simpleP1Age  ).c_str());
            remove((fileNameBase + simpleP1Dir  ).c_str());

            remove((fileNameBase + simpleP2Name ).c_str());
            remove((fileNameBase + simpleP2NameI).c_str());
            remove((fileNameBase + simpleP2Age  ).c_str());
            remove((fileNameBase + simpleP2Dir  ).c_str());

            remove((fileNameBase + simpleIndex  ).c_str());

            remove((fileNameBase                ).c_str());
        }
};

class SimpleTestDir: public TwoPeopleTest
{
    public:
        SimpleTestDir()
            : TwoPeopleTest(simpleTestDir)
        {}
};
class LockedTestDir: public TwoPeopleTest
{
    public:
        LockedTestDir()
            : TwoPeopleTest(lockedTestDir)
        {}
        void SetUp() override
        {
            mkdir(lockedTestDir.c_str(), 0'000);
        }
};
class LockedFileDir: public TwoPeopleTest
{
    public:
        LockedFileDir()
            : TwoPeopleTest(lockedFileDir)
        {}
        void SetUp() override
        {
            mkdir(lockedFileDir.c_str(), 0'777);
            mkdir((lockedFileDir + simpleP2Dir).c_str(), 0'777);
            int fd = open((lockedFileDir + simpleP2Name).c_str(), O_RDWR | O_CREAT, 0'000);
            close(fd);
        }
};

#endif
