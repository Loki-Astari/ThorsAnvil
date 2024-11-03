#ifndef THORSANVIL_THORSSOCKET_SOCKET_UTIL_H
#define THORSANVIL_THORSSOCKET_SOCKET_UTIL_H

#include "ThorsSocketConfig.h"
#include "SecureSocketUtil.h"

#include <cstddef>
#include <string_view>

#ifdef  __WINNT__
#include <winsock2.h>
#include <windows.h>
#define SOCKET_TYPE             SOCKET
#define THOR_BINARY             _O_BINARY
#else
#define SOCKET_TYPE             int
#define THOR_BINARY             0
#endif

namespace ThorsAnvil::ThorsSocket
{

using YieldFunc     = std::function<bool()>;

struct IOData
{
    std::size_t     dataSize;
    bool            stillOpen;
    bool            blocked;
};

enum class FileMode     {Read, WriteAppend, WriteTruncate};
enum class Blocking     {No, Yes};
enum class Mode         {Read, Write};
enum class DeferAccept  {No, Yes};

struct FileInfo
{
    std::string_view    fileName;
    FileMode            mode;
};

struct PipeInfo
{};

struct SocketInfo
{
    std::string_view    host;
    int                 port;
};

struct ServerInfo
{
    int                 port;
};

struct OpenSocketInfo
{
    SOCKET_TYPE         fd;
};

struct SSocketInfo: public SocketInfo
{
    SSLctx const&       ctx;
    DeferAccept         defer;
};

struct SServerInfo: public ServerInfo
{
    SSLctx              ctx;
};

struct OpenSSocketInfo: public OpenSocketInfo
{
    SSLctx const&       ctx;
    DeferAccept         defer;
};

}

#endif
