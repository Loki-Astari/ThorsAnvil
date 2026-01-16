#ifndef THORSANVIL_SERIALIZE_LOGGING_H
#define THORSANVIL_SERIALIZE_LOGGING_H

#include "SerializeConfig.h"

#define THORS_LOG_DATA_SERIAL                     ThorsAnvil::Serialize::PrinterConfig{ThorsAnvil::Serialize::OutputType::Stream}
#define THORS_LOG_DATA(D)                         "|Data|", ThorsAnvil::Serialize::jsonExporter(D, THORS_LOG_DATA_SERIAL)

#define ThorsLogAndThrowFatalWithData(E, D, ...)   ThorsLogAndThrowFatal(E,   __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowErrorWithData(E, D, ...)   ThorsLogAndThrowError(E,   __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowWarningWithData(E, D, ...) ThorsLogAndThrowWarning(E, __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowInfoWithData(E, D, ...)    ThorsLogAndThrowInfo(E,    __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowDebugWithData(E, D, ...)   ThorsLogAndThrowDebug(E,   __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowTraceWithData(E, D, ...)   ThorsLogAndThrowTrace(E,   __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAndThrowAllWithData(E, D, ...)     ThorsLogAndThrowAll(E,     __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogFatalWithData(D, ...)              ThorsLogFatal(  __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogErrorWithData(D, ...)              ThorsLogError(  __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogWarningWithData(D, ...)            ThorsLogWarning(__VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogInfoWithData(D, ...)               ThorsLogInfo(   __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogDebugWithData(D, ...)              ThorsLogDebug(  __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogTraceWithData(D, ...)              ThorsLogTrace(  __VA_ARGS__, THORS_LOG_DATA(D))
#define ThorsLogAllWithData(D, ...)                ThorsLogAll(    __VA_ARGS__, THORS_LOG_DATA(D))

#endif
