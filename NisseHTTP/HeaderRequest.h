#ifndef THORSANVIL_NISSE_NISSEHTTP_HEADER_REQUEST_H
#define THORSANVIL_NISSE_NISSEHTTP_HEADER_REQUEST_H

#include "NisseHTTPConfig.h"
#include <ostream>
#include <map>
#include <vector>
#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::HTTP
{

class HeaderRequest
{
    using HeaderMap = std::map<std::string, std::vector<std::string>>;
    using CIterator = HeaderMap::const_iterator;

    HeaderMap   headers;

    public:
        CIterator                       begin()                                 const   {return std::begin(headers);}
        CIterator                       end()                                   const   {return std::end(headers);}
        bool                            hasHeader(std::string_view header)      const   {return hasHeader(std::string(header));}
        bool                            hasHeader(std::string const& header)    const   {return headers.find(header) != headers.end();}
        std::vector<std::string> const& getHeader(char const* header)           const   {return getHeader(std::string(header));}
        std::vector<std::string> const& getHeader(std::string_view header)      const   {return getHeader(std::string(header));}
        std::vector<std::string> const& getHeader(std::string const& header)    const;

        bool operator==(HeaderRequest const& rhs)  const {return headers == rhs.headers;}
        bool operator!=(HeaderRequest const& rhs)  const {return !(*this == rhs);}

        void add(std::string_view header, std::string_view value);

        friend std::ostream& operator<<(std::ostream& stream, HeaderRequest const& headers) {headers.print(stream);return stream;}
        void print(std::ostream& stream) const;
    private:
        bool dedupHeader(std::string_view header);
        bool splitOnComma(std::string_view header);
        std::string_view getValue(std::string_view input);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "HeaderRequest.source"
#endif

#endif
