#ifndef THORS_ANVIL_LOGGING_THORS_LOGGING_H
#define THORS_ANVIL_LOGGING_THORS_LOGGING_H

#include "ThorsLoggingConfig.h"


#ifndef LOGURU_WITH_STREAMS
#define LOGURU_WITH_STREAMS 1
#endif

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
// Don't use logru with header only
#include <iostream>
#define FATAL       2
#define ERROR       3
#define WARNING     4
#define INFO        5
#ifndef THOR_LOGGING_DEFAULT_LOG_LEVEL
#define THOR_LOGGING_DEFAULT_LOG_LEVEL   3
#endif
#else
#include "loguru.hpp"
#endif
#include "ThorsIOUtil/Utility.h"
#include <stdexcept>
#include <string>


namespace ThorsAnvil
{
    namespace Logging
    {

    // This is for data/application corruption that requires the
    // application to immediately quit.
    class FatalException: public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
    // This is for situations that "should not" happen.
    // Use this exception for code paths that should not be reachable.
    //
    // This will help find issues that can be corrected by unit tests.
    // But occasionally will happen in production. You should only
    // catch this type of error at an action type level to abandon the
    // action.
    //
    // Note: This does not mean there has been data corruptions so we
    //       can potentially continue. The application writer must
    //       make that determination.
    class CriticalException: public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
    // This is for situations where you don't want to happen
    // but is caused because the input data does not conform
    // to expect patterns.
    class LogicalException: public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };

    }
}

#if HEADER_ONLY
#define ThorsLogAndThrowAction(Level, Exception, Scope, Function, ...)  \
do                                                                      \
{                                                                       \
    std::string message = ThorsAnvil::Utility::buildErrorMessage(       \
                                            Scope,                      \
                                            Function,                   \
                                            __VA_ARGS__);               \
    if (Level <= THOR_LOGGING_DEFAULT_LOG_LEVEL) {                      \
        std::cerr << message;                                           \
    }                                                                   \
    throw Exception(message);                                           \
}                                                                       \
while (false)
#define ThorsMessage(Level, ...)                                        \
do                                                                      \
{                                                                       \
    if (Level <= THOR_LOGGING_DEFAULT_LOG_LEVEL) {                      \
        std::cerr << ThorsAnvil::Utility::buildErrorMessage(__VA_ARGS__); \
    }                                                                   \
}                                                                       \
while (false)
#else
#define ThorsLogAndThrowAction(Level, Exception, Scope, Function, ...)  \
do                                                                      \
{                                                                       \
    std::string message = ThorsAnvil::Utility::buildErrorMessage(       \
                                            Scope,                      \
                                            Function,                   \
                                            __VA_ARGS__);               \
    LOG_F(Level, "%s", message.c_str());                                \
    throw Exception(message);                                           \
}                                                                       \
while (false)

#define ThorsMessage(Level, ...)        VLOG_S(Level) << ThorsAnvil::Utility::buildErrorMessage(__VA_ARGS__)
#endif


#define ThorsLogAndThrowFatal(...)      ThorsLogAndThrowAction(FATAL,   ThorsAnvil::Logging::FatalException, __VA_ARGS__)
#define ThorsLogAndThrowCritical(...)   ThorsLogAndThrowAction(ERROR,   ThorsAnvil::Logging::CriticalException, __VA_ARGS__)
#define ThorsLogAndThrowLogical(...)    ThorsLogAndThrowAction(WARNING, ThorsAnvil::Logging::LogicalException, __VA_ARGS__)
#define ThorsLogAndThrow(...)           ThorsLogAndThrowAction(2, std::runtime_error, __VA_ARGS__)

#define ThorsCatchMessage(S, F, e)      ThorsMessage(2, S, F, "Caught Exception: ", e)
#define ThorsRethrowMessage(S, F, e)    ThorsMessage(2, S, F, "ReThrow Exception: ",e)

#endif
