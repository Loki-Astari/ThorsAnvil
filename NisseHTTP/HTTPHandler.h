#ifndef THORSANVIL_NISSE_NISSEHTTP_HTTP_HANDLER_H
#define THORSANVIL_NISSE_NISSEHTTP_HTTP_HANDLER_H

#include "NisseHTTPConfig.h"
#include "PyntHTTP.h"
#include "PathMatcher.h"
#include <string>
#include <string_view>
#include <istream>
#include <utility>

namespace ThorsAnvil::Nisse::HTTP
{

class HeaderRequest;

class HTTPHandler: public PyntHTTP
{
    PathMatcher             pathMatcher;

    public:
        virtual void       processRequest(Request& request, Response& response) override;

        void addPath(std::string const& path, HTTPAction&& action, HTTPValidate&& val = [](Request const&){return true;}) {addPath(All::Method, path, std::move(action), std::move(val));}
        void remPath(std::string const& path)                                                                             {remPath(All::Method, path);}

        void addPath(MethodChoice method, std::string const& path, HTTPAction&& action, HTTPValidate&& val = [](Request const&){return true;});
        void remPath(MethodChoice method, std::string const& path);
    private:
        std::string normalize(std::string_view path);

        void addHeaders(RequestVariables& var, HeaderRequest const& headers);
        void addQueryParam(RequestVariables& var, std::string_view query);
        void addPathMatch(RequestVariables& var, Match const& matches);
        void addFormVariables(RequestVariables& var, std::istream& stream);

        bool callUserACtion(std::string const& path, HTTPAction& action, HTTPValidate& val, Match const& matches, Request& request, Response& response);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#include "HTTPHandler.source"
#endif

#endif
