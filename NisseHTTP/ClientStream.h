#ifndef THORSANVIL_NISSE_NISSEHTTP_ClientSSTREAM_H
#define THORSANVIL_NISSE_NISSEHTTP_ClientSSTREAM_H

#include "NisseHTTPConfig.h"
#include "ThorsSocket/SocketStream.h"

#include <string>
#include <iostream>

namespace ThorsAnvil::Nisse::HTTP
{

class HTTPRequest;

class ClientStream
{
    ThorsAnvil::ThorsSocket::SSLctx            ctx;
    ThorsAnvil::ThorsSocket::SocketStream      stream;

    static std::string getHost(std::string const& url);
    static int getPort(std::string const& url);

    public:
        ClientStream(std::string const& url);

        friend std::ostream& operator<<(ClientStream& stream, HTTPRequest const& request);

        operator std::ostream&() {return stream;}
        operator std::istream&() {return stream;}
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "ClientStream.source"
#endif

#endif
