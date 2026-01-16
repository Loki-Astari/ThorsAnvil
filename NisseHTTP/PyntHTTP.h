#ifndef THORSANVIL_NISSE_NISSEHTTP_PYNTHTTP_H
#define THORSANVIL_NISSE_NISSEHTTP_PYNTHTTP_H

/*
 * An HTTP implementation of Pynt
 * Version 1:
 */

#include "NisseHTTPConfig.h"
#include "NisseServer/Pynt.h"
#include <ThorsSocket/SocketStream.h>

namespace TASock = ThorsAnvil::ThorsSocket;

namespace ThorsAnvil::Nisse::HTTP
{

class Request;
class Response;

class PyntHTTP: public Server::Pynt
{
    public:
        virtual Server::PyntResult  handleRequest(TASock::SocketStream& stream, Server::Context& context) override;
        virtual void                processRequest(Request& request, Response& response) = 0;
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#endif

#endif
