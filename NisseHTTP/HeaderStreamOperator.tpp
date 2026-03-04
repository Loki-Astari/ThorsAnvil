#ifndef THORSANVIL_NISSE_NISSEHTTP_HEADER_STREAM_OPERATOR_TPP
#define THORSANVIL_NISSE_NISSEHTTP_HEADER_STREAM_OPERATOR_TPP

#include "HeaderResponse.h"
#include "HeaderPassThrough.h"
#include <ostream>

namespace ThorsAnvil::Nisse::HTTP
{
    // Needs to be inline.
    // Defined here and HeaderResponse.cpp
    inline
    std::ostream& operator<<(std::ostream& stream, Header const& header)
    {
        struct HeaderStream
        {
            std::ostream& stream;
            HeaderStream(std::ostream& stream)
                : stream(stream)
            {}
            std::ostream& operator()(HeaderResponse const& header)    {return stream << header;}
            std::ostream& operator()(HeaderPassThrough const& header) {return stream << header;}
        };
        return std::visit(HeaderStream{stream}, header);
    }

}

#endif
