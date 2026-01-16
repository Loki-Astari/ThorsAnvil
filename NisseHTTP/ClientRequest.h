#ifndef THORSANVIL_NISSE_NISSEHTTP_CLIENTREQUEST_H
#define THORSANVIL_NISSE_NISSEHTTP_CLIENTREQUEST_H

#include "NisseHTTPConfig.h"
#include "Util.h"
#include "StreamOutput.h"
#include <ostream>
#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::HTTP
{

class ClientRequest
{
    static std::string getHost(std::string const& url);
    static std::string_view getRequest(std::string const& url);

    protected:
        Method              method;
        Version             version;
        std::string         url;
        std::ostream&       baseStream;
        StreamOutput        stream;
        bool                headerSent;
    private:
        void flushHeaderIfNeeded();

    public:
        ClientRequest(std::ostream& baseStream, std::string url, Method method = Method::GET, Version version = Version::HTTP1_1);
        ~ClientRequest();

        void addHeaders(Header const& headers);
        std::ostream& body(BodyEncoding bodyEncoding);

        void flushRequest();
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "ClientRequest.source"
#endif

#endif
