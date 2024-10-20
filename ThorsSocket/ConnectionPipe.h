#ifndef THORSANVIL_THORSSOCKET_CONNECTION_PIPE_H
#define THORSANVIL_THORSSOCKET_CONNECTION_PIPE_H

#include "ThorsSocketConfig.h"
#include "ConnectionFileDescriptor.h"

namespace ThorsAnvil::ThorsSocket::ConnectionType
{

class Pipe: public FileDescriptor
{
    int fd[2];
    public:
        Pipe(PipeInfo const& pipeInfo, Blocking blocking);
        Pipe(int fd[]);
        virtual ~Pipe();

        virtual bool isConnected()                          const   override;
        virtual int  socketId(Mode rw)                      const   override;
        virtual void close()                                        override;

        virtual int getReadFD()                             const   override;
        virtual int getWriteFD()                            const   override;
        virtual void release()                                      override;
        virtual std::string_view protocol() const override {return "pipe";}
};

}

#if THORS_SOCKET_HEADER_ONLY
#include "ConnectionPipe.source"
#endif

#endif
