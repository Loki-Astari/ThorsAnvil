#ifndef THORSANVIL_NISSE_NISSEHTTP_URL_H
#define THORSANVIL_NISSE_NISSEHTTP_URL_H

#include "NisseHTTPConfig.h"
#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::HTTP
{

class URL
{
    std::string         hrefValue;
    std::string_view    protocolRef;
    std::string_view    originRef;
    std::string_view    hostRef;
    std::string_view    portRef;
    std::string_view    hostnameRef;
    std::string_view    pathRef;
    std::string_view    queryRef;
    std::string_view    hashRef;

    public:
        URL()   {}
        URL(std::string_view href);
        URL(std::string_view prot, std::string_view host, std::string_view request);

        URL(URL const& copy);
        URL(URL&& move) noexcept;
        URL& operator=(URL copyORmove) noexcept;

        bool operator==(URL const& rhs)     const {return hrefValue == rhs.hrefValue;}
        bool operator!=(URL const& rhs)     const {return !(*this == rhs);}

        std::string_view        href()      const {return hrefValue;}     // 'http://localhost:53/status?name=ryan#234'
        std::string_view        protocol()  const {return protocolRef;}   // 'http:'
        std::string_view        origin()    const {return originRef;}     // 'http://localhost:53'
        std::string_view        host()      const {return hostRef;}       // 'localhost:53',
        std::string_view        hostname()  const {return hostnameRef;}   // 'localhost',
        std::string_view        port()      const {return portRef;}       // '53'
        std::string_view        pathname()  const {return pathRef;}       // '/status',
        std::string_view        query()     const {return queryRef;}      // '?name=ryan',
        std::string_view        hash()      const {return hashRef;}       // '#234'

        std::string_view        param(std::string_view param);  // parm('name') => 'ryan'

        void swap(URL& other) noexcept;
        friend void swap(URL& lhs, URL& rhs) noexcept {lhs.swap(rhs);}
    private:
        static std::string buildHref(std::string_view prot, std::string_view host, std::string_view request);
        std::string_view findProtocol(std::string const& src);
        std::string_view findOrigin(std::string const& src);
        std::string_view findHost(std::string const& src);
        std::string_view findHostname(std::string const& src);
        std::string_view findPort(std::string const& src);
        std::string_view findPath(std::string const& src);
        std::string_view findQuery(std::string const& src);
        std::string_view findHash(std::string const& src);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "URL.source"
#endif

#endif
