#ifndef THORSANVIL_NISSE_NISSEHTTP_PATH_MATCHER_H
#define THORSANVIL_NISSE_NISSEHTTP_PATH_MATCHER_H

#include "NisseHTTPConfig.h"
#include "Util.h"
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <memory>
#include <utility>
#include <tuple>

namespace ThorsAnvil::Nisse::HTTP
{

class Request;
class Response;
using HTTPValidate  = std::function<bool(Request const& request)>;
using Match         = std::map<std::string, std::string>;
using HTTPAction    = std::function<bool(Request const& request, Response& response)>;

class HTTPHandler;
class PathMatcher
{
    public:
        struct Data
        {
            Data(HTTPHandler* parent, std::string const& path, HTTPAction&& action, HTTPValidate&& val)
                : parent(parent)
                , path(path)
                , action(std::move(action))
                , val(std::move(val))
            {}
            virtual ~Data() {}
            HTTPHandler*    parent;
            std::string     path;
            HTTPAction      action;
            HTTPValidate    val;
        };
    private:

    /*
     * Note: Using this definition of "Action"
     * //using Action    = std::function<bool(Match const&, Request&, Response&)>;
     *
     * Caused issues with dynamically closing shared libraries (via ::dlclose())
     * The stored "action" in the "paths" variable below caused dlclose() to crash on mac.
     * I could not find the exact cause.
     * I changed the definition to store a function pointer and a Data object solved this
     * issue.
     *      see: git hash 1cffd90961071d54c71a67e9151a25994f153d5f
     *      for the previous version.
     */
    using Action    = bool(*)(Data&, Match const&, Request&, Response&);
    using NameList  = std::vector<std::string>;
    using MatchList = std::vector<std::string>;

    struct MatchInfo
    {
        MethodChoice            method;
        MatchList               matchSections;
        NameList                names;
        Action                  action;
        std::unique_ptr<Data>   data;
    };
    struct MatchBase
    {
        MethodChoice    method;
        MatchList       matchSections;
        NameList        names;

        bool operator==(MatchInfo const& rhs) const
        {
            return std::tie(method, matchSections, names) == std::tie(rhs.method, rhs.matchSections, rhs.names);
        }
    };

    std::vector<MatchInfo>  paths;

    public:
        void addPath(MethodChoice method, std::string pathMatch, Action action, std::unique_ptr<Data> data);
        void remPath(MethodChoice method, std::string pathMatch);

        bool findMatch(std::string_view path, Request& request, Response& response);
    private:
        MatchBase   buildMatchInfo(MethodChoice method, std::string pathMatch);
        bool        checkPathMatch(MatchInfo const& pathMatchInfo, std::string_view path, Request& request, Response& response);
        std::string decode(std::string_view matched);
};

}

#if defined(NISSE_HEADER_ONLY) && NISSE_HEADER_ONLY == 1
#endif

#endif
