#ifndef THORSANVIL_THORSSOCKET_CONNECTION_WRAPPER_H
#define THORSANVIL_THORSSOCKET_CONNECTION_WRAPPER_H

#include "ThorsSocketConfig.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>


#ifdef  __WINNT__
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define PAUSE_AND_WAIT(n)       Sleep(n * 1000)
#define NONBLOCKING_FLAG        0
#define getErrNoStrSocket       getErrNoStrWin
#define getErrMsgSocket         getErrMsgWin
#define THOR_POLL_TYPE          WSAPOLLFD
#define THOR_POLL               WSAPoll
#define THOR_SOCKET_ID(x)       static_cast<SOCKET>(x)
#define THOR_POLL_ERROR         SOCKET_ERROR
#define THOR_POLLPRI            0

int thorCreatePipe(int fd[2]);
int thorSetFDNonBlocking(int fd);
int thorSetSocketNonBlocking(SOCKET fd);
int thorCloseSocket(SOCKET fd);
int thorShutdownSocket(SOCKET fd);
inline int thorGetSocketError()             {return WSAGetLastError();}
inline SOCKET thorInvalidFD()               {return INVALID_SOCKET;}
inline bool thorErrorIsTryAgain(int error)  {return error == WSATRY_AGAIN;}
char const* getErrNoStrWin(int error);
char const* getErrMsgWin(int error);

#else
#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/uio.h>
#include <netdb.h>
#include <errno.h>

#define PAUSE_AND_WAIT(n)       sleep(n)
#define NONBLOCKING_FLAG        O_NONBLOCK
#define getErrNoStrSocket       getErrNoStrUnix
#define getErrMsgSocket         getErrMsgUnix
#define THOR_POLL_TYPE          pollfd
#define THOR_POLL               poll
#define THOR_SOCKET_ID(x)       x
#define THOR_POLL_ERROR         -1
#define THOR_POLLPRI            POLLPRI

int thorCreatePipe(int fd[2]);
int thorSetFDNonBlocking(int fd);
int thorSetSocketNonBlocking(int fd);
int thorCloseSocket(int fd);
int thorShutdownSocket(int fd);
inline int thorGetSocketError()             {return errno;}
inline int thorInvalidFD()                  {return -1;}
inline bool thorErrorIsTryAgain(int error)  {return error == TRY_AGAIN;}

#endif

// Used in both Win and Unix versions
char const* getErrNoStrUnix(int error);
char const* getErrMsgUnix(int error);

inline STACK_OF(X509_NAME)* sk_X509_NAME_new_null_wrapper()                            {return sk_X509_NAME_new_null();}
inline void                 sk_X509_NAME_free_wrapper(STACK_OF(X509_NAME)* list)       {sk_X509_NAME_free(list);}
inline void                 sk_X509_NAME_pop_free_wrapper(STACK_OF(X509_NAME)* list)   {sk_X509_NAME_pop_free(list, X509_NAME_free);}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionUtil.source"
#endif

#endif
