#ifndef THORSANVIL_NISSE_NISSEHTTP_CLIENTRESPONSE_H
#define THORSANVIL_NISSE_NISSEHTTP_CLIENTRESPONSE_H

#include "NisseHTTPConfig.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <map>
#include <cstddef>

namespace ThorsAnvil::Nisse::HTTP
{

struct StatusResponse
{
    int                 code;
    std::string         message;
};

class ClientResponse
{
    protected:
        Version         version;
        StatusResponse  status;
        std::map<std::string, std::string>  headers;

    public:
        ClientResponse(std::istream& baseStream);
        void print(std::ostream& stream) const;
        std::size_t getContentSize() const;
        friend std::ostream& operator<<(std::ostream& stream, ClientResponse const& data) {data.print(stream);return stream;}
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "ClientResponse.source"
#endif

#endif
