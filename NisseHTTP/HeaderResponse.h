#ifndef THORSANVIL_NISSE_NISSEHTTP_HEADER_RESPONSE_H
#define THORSANVIL_NISSE_NISSEHTTP_HEADER_RESPONSE_H

#include "NisseHTTPConfig.h"
#include <string>
#include <string_view>
#include <map>
#include <ostream>

namespace ThorsAnvil::Nisse::HTTP
{

class HeaderResponse
{
    std::map<std::string, std::string>  headers;
    public:
        bool    empty() const;
        void    add(std::string_view header, std::string_view value);

    friend std::ostream& operator<<(std::ostream& stream, HeaderResponse const& headersBlock)   {headersBlock.print(stream);return stream;}

    void print(std::ostream& stream) const;
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "HeaderResponse.source"
#endif

#endif
