#ifndef THORSANVIL_NISSE_CORE_UTILITY_UTILITY_H
#define THORSANVIL_NISSE_CORE_UTILITY_UTILITY_H

#include "ThorsIOUtilConfig.h"
#include <string>
#include <sstream>
#include <ostream>
#include <cstdio>
#include <cstring>

namespace ThorsAnvil
{
    namespace Utility
    {

/* Used to preserve a streams format state */
/* Usage:
 *     stream << StreamFormatterNoChange{} << std::hex << std::setprecision(23) << value;
 *     // After the expression is complete the state of the stream is reverted to its original state.
 */
class StreamFormatterNoChange
{
    mutable std::ios*               stream;
    mutable std::ios_base::fmtflags flags;
    mutable std::streamsize         precision;
    public:
        StreamFormatterNoChange()
            : stream(nullptr)
        {}
        ~StreamFormatterNoChange()
        {
            if (stream)
            {
                stream->flags(flags);
                stream->precision(precision);
            }
        }
        void saveStream(std::ios& s) const
        {
            stream    = &s;
            flags     = s.flags();
            precision = s.precision();
        }
        friend std::ostream& operator<<(std::ostream& stream, StreamFormatterNoChange const& formatter)
        {
            formatter.saveStream(stream);
            return stream;
        }
        friend std::istream& operator>>(std::istream& stream, StreamFormatterNoChange const& formatter)
        {
            formatter.saveStream(stream);
            return stream;
        }
};

template<typename... Args>
inline
void print(std::ostream& s, Args&... args)
{
    using Expander = int[];
    Expander ignore{ 0, (s << args, 0)...};
    (void)ignore; // Cast to ignore to avoid compiler warnings.
}

template<typename... Args>
inline
std::string buildStringFromParts(Args const&... args)
{
    std::stringstream msg;
    print(msg, args...);
    return msg.str();
}

// @function
inline
std::size_t getUniqueErrorId()
{
    static std::size_t  errorMessageId = 0;
    return errorMessageId++;
}

// @function
template<typename... Args>
inline
std::string buildErrorMessage(char const* className, char const* method, Args const&... args)
{
    return buildStringFromParts("id: ", getUniqueErrorId(), " ", className, "::", method, ": ", args...);
}

template<typename... Args>
inline
std::string buildBugReport(Args const& ...a)
{
    return buildErrorMessage(a..., "\nPlease File a Bug Report: ");
}

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
#ifndef __WINNT
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
    result += std::strerror(errno);
    return result;
}

    }
}

#endif
