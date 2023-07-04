#include "gtest/gtest.h"
#include "ThorsLogging/ThorsLogging.h"
#include <iostream>
#include <string>



class LoggingEnvironment: public ::testing::Environment
{
    public:
    ~LoggingEnvironment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override
    {
        using namespace std::string_literals;

        char* logging = getenv("THOR_LOG_LEVEL");
        if (logging == nullptr)
        {
            std::cerr << "NO - THOR_LOG_LEVEL\n";
        }
        else
        {
            std::cerr << "Logging Level BEFORE: " << loguru::g_stderr_verbosity << "\n";
            std::cerr << "THOR_LOG_LEVEL = " << logging << "\n";
            int level = std::strtol(logging, nullptr, 10);
            if (level > 0 && level <10)
            {
                loguru::g_stderr_verbosity = level;
            }
            else if ("FATAL"s == logging)
            {
                loguru::g_stderr_verbosity = loguru::Verbosity_FATAL;
            }
            else if ("ERROR"s == logging)
            {
                loguru::g_stderr_verbosity = loguru::Verbosity_ERROR;
            }
            else if ("WARNING"s == logging)
            {
                loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
            }
            else if ("INFO"s == logging)
            {
                loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
            }
            else if ("DEBUG"s == logging)
            {
                loguru::g_stderr_verbosity = 5;
            }
            else if ("ALL"s == logging)
            {
                loguru::g_stderr_verbosity = 9;
            }
            std::cerr << "Logging Level AFTER: " << loguru::g_stderr_verbosity << "\n";
        }
    }

    // Override this to define how to tear down the environment.
    void TearDown() override {}
};

bool initLogging()
{
    std::cerr << "Logging Init\n";
    ::testing::Environment* const foo_env = ::testing::AddGlobalTestEnvironment(new LoggingEnvironment);
    ((void)foo_env);
    return true;
}

bool logingInit = initLogging();

