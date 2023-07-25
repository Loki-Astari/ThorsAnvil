#ifndef THORSANVIL_THORSSOCKET_CONNECTION_FILE_DESCRIPTOR_H
#define THORSANVIL_THORSSOCKET_CONNECTION_FILE_DESCRIPTOR_H

#include "ThorsSocketConfig.h"
#include "Connection.h"

#include  <cstddef>

namespace ThorsAnvil::ThorsSocket::ConnectionType
{

class FileDescriptor: public Connection
{
    public:
        virtual void tryFlushBuffer()                                       override;

        virtual IOData readFromStream(char* buffer, std::size_t size)       override;
        virtual IOData writeToStream(char const* buffer, std::size_t size)  override;

        char const* getErrNoStr(int error);
    protected:
        virtual int getReadFD()                             const = 0;
        virtual int getWriteFD()                            const = 0;
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionFileDescriptor.source"
#endif

#endif
