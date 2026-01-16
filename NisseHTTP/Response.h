#ifndef THORSANVIL_NISSE_NISSEHTTP_RESPONSE_H
#define THORSANVIL_NISSE_NISSEHTTP_RESPONSE_H

#include "NisseHTTPConfig.h"
#include "StreamOutput.h"
#include <iostream>
#include <string_view>
#include <chrono>

namespace ThorsAnvil::Nisse::HTTP
{

class Response
{
    Version         version;
    StatusCode      statusCode;
    bool            headerSent;

    std::ostream&   baseStream;
    StreamOutput    stream;

    std::chrono::time_point<std::chrono::high_resolution_clock>  checkPoint;

    void sendHeaderIfNotSent();

    public:
        Response(std::ostream& stream, Version version, int code = 200);
        ~Response();

        // Read a response from another server.
        friend std::istream& operator>>(std::istream& stream, Response& response)  {response.read(stream);return stream;}
        void read(std::istream& stream);


        // Build up the response message.
        void          setStatus(int code);
        void          addHeaders(Header const& headers);
        std::ostream& body(BodyEncoding encoding);

        // Simplifications to handling common cases.
        void          error(int code, std::string_view errorMessage);   // Call(s) setStatus() adds A single header with the Error Message.

        StatusCode const& getCode() const {return statusCode;}
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "Response.source"
#endif

#endif
