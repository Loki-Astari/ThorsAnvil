#ifndef THORSANVIL_NISSE_NISSEHTTP_HEADER_PASS_THROUGH_H
#define THORSANVIL_NISSE_NISSEHTTP_HEADER_PASS_THROUGH_H

#include "NisseHTTPConfig.h"
#include "Util.h"
#include <iostream>
#include <string_view>

namespace ThorsAnvil::Nisse::HTTP
{

class HeaderPassThrough
{
    std::istream&   src;
    mutable BodyEncoding    encoding;

    public:
        HeaderPassThrough(std::istream& stream);
        BodyEncoding getEncoding() const    {return encoding;}

        friend std::ostream& operator<<(std::ostream& stream, HeaderPassThrough& header)  {header.print(stream);return stream;}
        void print(std::ostream& stream);
    private:
        bool isContentLength(std::string_view line) const;
        bool isTransferEncoding(std::string_view line) const;
        std::string_view getValue(std::string_view input) const;
        std::streamsize getContentLength(std::string_view line) const;

};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "HeaderPassThrough.source"
#endif

#endif
