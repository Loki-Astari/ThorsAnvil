#ifndef THORSANVIL_THORS_MUG_MUG_PLUGIN_H
#define THORSANVIL_THORS_MUG_MUG_PLUGIN_H

#include "ThorsMugConfig.h"

#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
#include "NisseHTTP/HTTPHandler.h"

#include <string>
#include <vector>


namespace NisHttp   = ThorsAnvil::Nisse::HTTP;


typedef void (*MugHanlde)(ThorsAnvil::Nisse::HTTP::Request&, ThorsAnvil::Nisse::HTTP::Response&);

namespace ThorsAnvil::ThorsMug
{

class MugPlugin
{
    public:
        virtual ~MugPlugin()                                    {}
        virtual void start(NisHttp::HTTPHandler& handler)  = 0;
        virtual void stop(NisHttp::HTTPHandler& handler)  = 0;
};

struct Action
{
        NisHttp::Method         method;
        std::string             path;
        NisHttp::HTTPAction     action;
        NisHttp::HTTPValidate   validate = [](NisHttp::Request const&){return true;};
};

class MugPluginSimple: public MugPlugin
{
    public:
        virtual void start(NisHttp::HTTPHandler& handler) override
        {
            auto data = getAction();
            for (auto& item: data) {
                handler.addPath(item.method, item.path, std::move(item.action), std::move(item.validate));
            }
        }
        virtual void stop(NisHttp::HTTPHandler& handler) override
        {
            auto data = getAction();
            for (auto& item: data) {
                handler.remPath(item.method, item.path);
            }
        }

        virtual std::vector<Action> getAction() = 0;
};

extern "C"
{
    typedef ThorsAnvil::ThorsMug::MugPlugin*(*MugFunc)(char const* config);
}

}

#endif
