#ifndef THORSANVIL_LOGGING_THORS_LOGGING_H
#define THORSANVIL_LOGGING_THORS_LOGGING_H

#include "ThorsLoggingConfig.h"
#include <cstring>


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
    using Verbosity = int;
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

class ThorsLoggingLevel
{
    public:
    static loguru::Verbosity& logLevel()
    {
        static loguru::Verbosity stderr_verbosity = loguru::Verbosity_5;
        return stderr_verbosity;
    }
};


#define ThorsLogLevelSet(level)         ThorsLoggingLevel::logLevel() = level
#define ThorsLogLevel(level)            ThorsLoggingLevel::logLevel() = ThorsLogLevelItem(level)
#define ThorsLogLevelGet()              ThorsLoggingLevel::logLevel()

#else

#define ThorsLogOutput(Level, message)  LOG_F(Level, "%s", message_ThorsLogAndThrowAction.c_str())
#define ThorsLogLevelSet(level)         loguru::g_stderr_verbosity = level
#define ThorsLogLevel(level)            loguru::g_stderr_verbosity = ThorsLogLevelItem(level)
#define ThorsLogLevelGet()              loguru::g_stderr_verbosity

#endif

#define COUNT(...)              COUNT_(0, __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, Ignore)
#define COUNT_(I20, I19, I18, I17, I16, I15, I14, I13, I12, I11, I10, I9, I8, I7, I6, I5, I4, I3, I2, I1, I0, count, ...)                   count

#define STREAM_PARAM(...)           STREAM_PARAM_N(COUNT(__VA_ARGS__), __VA_ARGS__)
#define STREAM_PARAM_N(N, ...)      STREAM_PARAM_N_(N, __VA_ARGS__)
#define STREAM_PARAM_N_(N, ...)     STREAM_PARAM_ ## N(__VA_ARGS__)
#define STREAM_PARAM_1(Val)         Val
#define STREAM_PARAM_2(Val, ...)    Val << STREAM_PARAM_1(__VA_ARGS__)
#define STREAM_PARAM_3(Val, ...)    Val << STREAM_PARAM_2(__VA_ARGS__)
#define STREAM_PARAM_4(Val, ...)    Val << STREAM_PARAM_3(__VA_ARGS__)
#define STREAM_PARAM_5(Val, ...)    Val << STREAM_PARAM_4(__VA_ARGS__)
#define STREAM_PARAM_6(Val, ...)    Val << STREAM_PARAM_5(__VA_ARGS__)
#define STREAM_PARAM_7(Val, ...)    Val << STREAM_PARAM_6(__VA_ARGS__)
#define STREAM_PARAM_8(Val, ...)    Val << STREAM_PARAM_7(__VA_ARGS__)
#define STREAM_PARAM_9(Val, ...)    Val << STREAM_PARAM_8(__VA_ARGS__)
#define STREAM_PARAM_10(Val, ...)   Val << STREAM_PARAM_9(__VA_ARGS__)
#define STREAM_PARAM_11(Val, ...)   Val << STREAM_PARAM_10(__VA_ARGS__)
#define STREAM_PARAM_12(Val, ...)   Val << STREAM_PARAM_11(__VA_ARGS__)
#define STREAM_PARAM_13(Val, ...)   Val << STREAM_PARAM_12(__VA_ARGS__)
#define STREAM_PARAM_14(Val, ...)   Val << STREAM_PARAM_13(__VA_ARGS__)
#define STREAM_PARAM_15(Val, ...)   Val << STREAM_PARAM_14(__VA_ARGS__)
#define STREAM_PARAM_16(Val, ...)   Val << STREAM_PARAM_15(__VA_ARGS__)
#define STREAM_PARAM_17(Val, ...)   Val << STREAM_PARAM_16(__VA_ARGS__)
#define STREAM_PARAM_18(Val, ...)   Val << STREAM_PARAM_17(__VA_ARGS__)
#define STREAM_PARAM_19(Val, ...)   Val << STREAM_PARAM_18(__VA_ARGS__)
#define STREAM_PARAM_20(Val, ...)   Val << STREAM_PARAM_19(__VA_ARGS__)

#define ThorsAnvilUtilitybuildErrorMessage(c, m, ...)  STREAM_PARAM("id: ", getUniqueErrorIdX(), " ", c, "::", m, ": ", __VA_ARGS__)

inline
std::size_t getUniqueErrorIdX()
{
    static std::size_t  errorMessageId = 0;
    return errorMessageId++;
}


#define ThorsMessage(Level, ...)        VLOG_S(loguru::Verbosity_ ## Level) << ThorsAnvilUtilitybuildErrorMessage(__VA_ARGS__)
#define ThorsMessageStr(Level, str)     VLOG_S(loguru::Verbosity_ ## Level) << str


#define ThorsLogActionWithPotetialThrow(hasExcept, Exception, Level, Scope, Function, ...)          \
do                                                                      \
{                                                                       \
    if constexpr (!hasExcept)                                           \
    {                                                                   \
        ThorsMessage(Level, Scope, Function, __VA_ARGS__);              \
    }                                                                   \
    else                                                                \
    {                                                                   \
        std::stringstream message_ThorsLogAndThrowAction;               \
        message_ThorsLogAndThrowAction <<                               \
                          ThorsAnvilUtilitybuildErrorMessage(           \
                                            Scope,                      \
                                            Function,                   \
                                            __VA_ARGS__);               \
        ThorsMessageStr(Level, message_ThorsLogAndThrowAction.str());   \
        throw Exception(message_ThorsLogAndThrowAction.str());          \
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

#define ThorsLogLevelItem(level)        loguru::Verbosity_ ## level

#define ThorsCatchMessage(S, F, e)      ThorsMessage(2, S, F, "Caught Exception: ", e)
#define ThorsRethrowMessage(S, F, e)    ThorsMessage(2, S, F, "ReThrow Exception: ",e)

class ThorsLogTemp
{
    int oldLevel;
    public:
        ThorsLogTemp(loguru::NamedVerbosity level)
            : ThorsLogTemp(static_cast<int>(level))
        {}
        ThorsLogTemp(int level)
        {
            oldLevel = ThorsLogLevelGet();
            ThorsLogLevelSet(level);
        }
        ~ThorsLogTemp()
        {
            ThorsLogLevelSet(oldLevel);
        }
};


namespace ThorsAnvil::Utility
{

inline
std::string errnoToName()
{
    switch (errno)
    {
#define  THOR_CASE(x)  case x: return #x " "
        THOR_CASE( EACCES );
        THOR_CASE( EADDRINUSE );
        THOR_CASE( EADDRNOTAVAIL );
        THOR_CASE( EAFNOSUPPORT );
        THOR_CASE( EAGAIN );
        // TODO );
        //THOR_CASE( EWOULDBLOCK );
        THOR_CASE( EBADF );
        THOR_CASE( ECONNABORTED );
        THOR_CASE( ECONNRESET );
        THOR_CASE( EDESTADDRREQ );
#if !defined(__WINNT) && !defined(_WIN32) && !defined(_WIN64)
        THOR_CASE( EDQUOT );
#endif
        THOR_CASE( EEXIST );
        THOR_CASE( EFAULT );
        THOR_CASE( EFBIG );
        THOR_CASE( EINTR );
        THOR_CASE( EINVAL );
        THOR_CASE( EIO );
        THOR_CASE( EISDIR );
        THOR_CASE( ELOOP );
        THOR_CASE( EMFILE );
        THOR_CASE( ENAMETOOLONG );
        THOR_CASE( ENETDOWN );
        THOR_CASE( ENETUNREACH );
        THOR_CASE( ENFILE );
        THOR_CASE( ENOBUFS );
        THOR_CASE( ENOENT );
        THOR_CASE( ENOMEM );
        THOR_CASE( ENOSPC );
        THOR_CASE( ENOTCONN );
        THOR_CASE( ENOTDIR );
        THOR_CASE( ENOTSOCK );
        THOR_CASE( ENXIO );
        THOR_CASE( EOPNOTSUPP );
        THOR_CASE( EPIPE );
        THOR_CASE( EPROTONOSUPPORT );
        THOR_CASE( EPROTOTYPE );
        THOR_CASE( EROFS );
        THOR_CASE( ESPIPE );
        THOR_CASE( ETIMEDOUT );
        THOR_CASE( EALREADY );
        THOR_CASE( ECONNREFUSED );
        THOR_CASE( EHOSTUNREACH );
        THOR_CASE( EINPROGRESS );
        THOR_CASE( EISCONN );
#undef THOR_CASE
    }
    return "Unknown: ";
}

// @function
inline
std::string systemErrorMessage()
{
    std::string result = errnoToName();
#if !defined(_WIN32) && !defined(_WIN64)
    result += std::strerror(errno);
#else
    static char buffer[1000];
    strerror_s(buffer, 1000, errno);
    result += buffer;
#endif
    return result;
}

}

#endif
