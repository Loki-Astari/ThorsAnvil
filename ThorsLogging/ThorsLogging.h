#ifndef THORSANVIL_LOGGING_THORS_LOGGING_H
#define THORSANVIL_LOGGING_THORS_LOGGING_H

#include "ThorsLoggingConfig.h"


#ifndef LOGURU_WITH_STREAMS
#define LOGURU_WITH_STREAMS 1
#endif

#if defined(THORS_LOGGING_HEADER_ONLY) && THORS_LOGGING_HEADER_ONLY == 1

#ifndef THOR_LOGGING_DEFAULT_LOG_LEVEL
#define THOR_LOGGING_DEFAULT_LOG_LEVEL   3
#endif

#include <iostream>
namespace loguru
{
    enum NamedVerbosity : int
    {
        // Used to mark an invalid verbosity. Do not log to this level.
        Verbosity_INVALID = -10, // Never do LOG_F(INVALID)

        // You may use Verbosity_OFF on g_stderr_verbosity, but for nothing else!
        Verbosity_OFF     = -9, // Never do LOG_F(OFF)

        // Prefer to use ABORT_F or ABORT_S over LOG_F(FATAL) or LOG_S(FATAL).
        Verbosity_FATAL   = -3,
        Verbosity_ERROR   = -2,
        Verbosity_WARNING = -1,

        // Normal messages. By default written to stderr.
        Verbosity_INFO    =  0,

        // Same as Verbosity_INFO in every way.
        Verbosity_0       =  0,

        // Verbosity levels 1-9 are generally not written to stderr, but are written to file.
        Verbosity_1       = +1,
        Verbosity_2       = +2,
        Verbosity_3       = +3,
        Verbosity_4       = +4,
        Verbosity_5       = +5,
        Verbosity_6       = +6,
        Verbosity_7       = +7,
        Verbosity_8       = +8,
        Verbosity_9       = +9,

        // Do not use higher verbosity levels, as that will make grepping log files harder.
        Verbosity_MAX     = +9,
    };

}

#else
#include "loguru.hpp"
#endif
#include "ThorsIOUtil/Utility.h"
#include <stdexcept>
#include <string>


#if defined(THORS_LOGGING_HEADER_ONLY) && THORS_LOGGING_HEADER_ONLY == 1

class ConvertToVoid
{
    public:
        void operator&(std::ostream&) {}
};

#define VLOG_IF_S(verbosity, cond)                                                                 \
    ((verbosity) > THOR_LOGGING_DEFAULT_LOG_LEVEL || (cond) == false)                              \
        ? (void)0                                                                                  \
        : ConvertToVoid{} & std::cerr
#define VLOG_S(verbosity)              VLOG_IF_S(verbosity, true)

#define ThorsLogOutput(Level, message)                                  \
do                                                                      \
{                                                                       \
    if ((loguru::Verbosity_ ## Level) <= THOR_LOGGING_DEFAULT_LOG_LEVEL) {   \
        std::cerr << message;                                           \
    }                                                                   \
}                                                                       \
while (false)

#else

#define ThorsLogOutput(Level, message)  LOG_F(Level, "%s", message_ThorsLogAndThrowAction.c_str())

#endif

#define ThorsMessage(Level, ...)        VLOG_S(loguru::Verbosity_ ## Level) << ThorsAnvil::Utility::buildErrorMessage(__VA_ARGS__)


#define ThorsLogActionWithPotetialThrow(hasExcept, Exception, Level, Scope, Function, ...)          \
do                                                                      \
{                                                                       \
    ThorsMessage(Level, Scope, Function, __VA_ARGS__);                  \
    if constexpr (hasExcept)                                            \
    {                                                                   \
        std::string message_ThorsLogAndThrowAction =                    \
                          ThorsAnvil::Utility::buildErrorMessage(       \
                                            Scope,                      \
                                            Function,                   \
                                            __VA_ARGS__);               \
        throw Exception(message_ThorsLogAndThrowAction);                \
    }                                                                   \
}                                                                       \
while (false)

#define ThorsLogAndThrowAction(...)     ThorsLogActionWithPotetialThrow(true, __VA_ARGS__)
#define ThorsLogAction(...)             ThorsLogActionWithPotetialThrow(false, std::runtime_error, __VA_ARGS__)

#define ThorsLogAndThrowFatal(E, ...)   ThorsLogAndThrowAction(E,    FATAL, __VA_ARGS__)
#define ThorsLogAndThrowError(E, ...)   ThorsLogAndThrowAction(E,    ERROR, __VA_ARGS__)
#define ThorsLogAndThrowWarning(E, ...) ThorsLogAndThrowAction(E,  WARNING, __VA_ARGS__)
#define ThorsLogAndThrowInfo(E, ...)    ThorsLogAndThrowAction(E,     INFO, __VA_ARGS__)
#define ThorsLogAndThrowDebug(E, ...)   ThorsLogAndThrowAction(E,        3, __VA_ARGS__)
#define ThorsLogAndThrowTrack(E, ...)   ThorsLogAndThrowAction(E,        5, __VA_ARGS__)
#define ThorsLogAndThrowTrace(E, ...)   ThorsLogAndThrowAction(E,        7, __VA_ARGS__)
#define ThorsLogAndThrowAll(E, ...)     ThorsLogAndThrowAction(E,        9, __VA_ARGS__)
#define ThorsLogFatal(...)              ThorsLogAction(FATAL,   __VA_ARGS__)
#define ThorsLogError(...)              ThorsLogAction(ERROR,   __VA_ARGS__)
#define ThorsLogWarning(...)            ThorsLogAction(WARNING, __VA_ARGS__)
#define ThorsLogInfo(...)               ThorsLogAction(INFO,    __VA_ARGS__)
#define ThorsLogDebug(...)              ThorsLogAction(3,       __VA_ARGS__)
#define ThorsLogTrack(...)              ThorsLogAction(5,       __VA_ARGS__)
#define ThorsLogTrace(...)              ThorsLogAction(7,       __VA_ARGS__)
#define ThorsLogAll(...)                ThorsLogAction(9,       __VA_ARGS__)

#define ThorsCatchMessage(S, F, e)      ThorsMessage(2, S, F, "Caught Exception: ", e)
#define ThorsRethrowMessage(S, F, e)    ThorsMessage(2, S, F, "ReThrow Exception: ",e)

#endif
