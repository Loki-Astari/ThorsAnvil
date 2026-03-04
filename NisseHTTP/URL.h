#ifndef THORSANVIL_NISSE_NISSEHTTP_URL_H
#define THORSANVIL_NISSE_NISSEHTTP_URL_H

#include "NisseHTTPConfig.h"
#include <cstddef>
#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::HTTP
{

struct ViewPoint
{
    std::size_t         start   = 0;
    std::size_t         len     = 0;

    std::string_view    getView(std::string const& s) const     {return {&s[start], len};}
    std::size_t         size() const                            {return len;}

    void remove_prefix(std::size_t s)                           {start += s; len -= s;}
    void remove_suffix(std::size_t s)                           {len -= s;}

    std::size_t         find(std::string const& base, char c)   {return base.find(c, start) - start;}
};

class URL
{
    std::string         hrefValue;
    ViewPoint           protocolRef;
    ViewPoint           originRef;
    ViewPoint           hostRef;
    ViewPoint           portRef;
    ViewPoint           hostnameRef;
    ViewPoint           pathRef;
    ViewPoint           queryRef;
    ViewPoint           hashRef;

    public:
        URL()   {}
        URL(std::string_view href);
        URL(std::string_view prot, std::string_view host, std::string_view request);

        URL(URL const& copy);
        URL(URL&& move) noexcept;
        URL& operator=(URL copyORmove) noexcept;

        bool operator==(URL const& rhs)     const {return hrefValue == rhs.hrefValue;}
        bool operator!=(URL const& rhs)     const {return !(*this == rhs);}

        std::string_view        href()      const {return hrefValue;}                       // 'http://localhost:53/status?name=ryan#234'
        std::string_view        protocol()  const {return protocolRef.getView(hrefValue);}  // 'http:'
        std::string_view        origin()    const {return originRef.getView(hrefValue);}    // 'http://localhost:53'
        std::string_view        host()      const {return hostRef.getView(hrefValue);}      // 'localhost:53',
        std::string_view        hostname()  const {return hostnameRef.getView(hrefValue);}  // 'localhost',
        std::string_view        port()      const {return portRef.getView(hrefValue);}      // '53'
        std::string_view        pathname()  const {return pathRef.getView(hrefValue);}      // '/status',
        std::string_view        query()     const {return queryRef.getView(hrefValue);}     // '?name=ryan',
        std::string_view        hash()      const {return hashRef.getView(hrefValue);}      // '#234'

        std::string_view        param(std::string_view param);  // parm('name') => 'ryan'

        void swap(URL& other) noexcept;
        friend void swap(URL& lhs, URL& rhs) noexcept {lhs.swap(rhs);}
    private:
        static std::string buildHref(std::string_view prot, std::string_view host, std::string_view request);
        ViewPoint findProtocol(std::string const& src);
        ViewPoint findOrigin(std::string const& src);
        ViewPoint findHost(std::string const& src);
        ViewPoint findHostname(std::string const& src);
        ViewPoint findPort(std::string const& src);
        ViewPoint findPath(std::string const& src);
        ViewPoint findQuery(std::string const& src);
        ViewPoint findHash(std::string const& src);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "URL.source"
#endif

#endif
