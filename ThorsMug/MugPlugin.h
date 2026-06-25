#ifndef THORSANVIL_THORS_MUG_MUG_PLUGIN_H
#define THORSANVIL_THORS_MUG_MUG_PLUGIN_H

#include "ThorsMugConfig.h"

#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
#include "NisseHTTP/HTTPHandler.h"
#include "ThorSerialize/JsonThor.h"

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

    private:
        // Friend declaration so mugInterface can register robots.
        template<typename Server, typename ServerConfig>
        friend ThorsAnvil::ThorsMug::MugPlugin* mugCreateSimpleInstance(int init, char const* configStr);

        // Addig and removing Bots at runtime.
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* add(ServerConfig&& serverConfig);
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* remomve(ServerConfig&& serverConfig);

        // Currently active Bots.
        static std::map<std::string, std::unique_ptr<MugPluginSimple>>& getServerInfo()
        {
            static std::map<std::string, std::unique_ptr<MugPluginSimple>>  apps;
            return apps;
        }

};

/*
 * This function is what is exported from the DLL.
 * This is to maintain the "C" only interface.
 *
 * We return a "Non Owned" pointer to MugPlugin object.
 * The DLLib class will call this function to retrive the pointer.
 * It (DLLib) will call "start()"/"stop()" with each load/unload cycle.
 */
extern "C"
{
    typedef ThorsAnvil::ThorsMug::MugPlugin*(*MugFunc)(int initdest, char const* config);
}

/**
 * Template functions that need to be in header file
 */
template<typename Server, typename ServerConfig>
MugPlugin* MugPluginSimple::add(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<MugPluginSimple>>&    servers = getServerInfo();
    std::string const& slot = serverConfig.slot;
    // Try and find. Insert if it is not there.
    auto insert = servers.try_emplace(slot);
    auto find = std::get<0>(insert);
    if (find != servers.end() && find->second.get() != nullptr) {
        // Extracting the pointer here
        // So I can use it in typeid() without generating a warning message.
        MugPluginSimple* rawApp = find->second.get();
        ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::Bolt::App", "add", "Can not load Mug Server this slot >", slot, " is already being used. Current: ", typeid(*rawApp).name(), " New: ", typeid(Server).name());
    }
    find->second = std::make_unique<Server>(std::forward<ServerConfig>(serverConfig));
    return find->second.get();
}

template<typename Server, typename ServerConfig>
MugPlugin* MugPluginSimple::remomve(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<MugPluginSimple>>&    servers = getServerInfo();
    auto find = servers.find(serverConfig.slot);
    if (find != servers.end()) {
        find->second.reset();
    }
    return nullptr;
}

template<typename Server, typename ServerConfig>
MugPlugin* mugCreateSimpleInstance(int init, char const* configStr)
{
    ServerConfig    config = ThorsAnvil::Serialize::jsonBuilder<ServerConfig>(std::stringstream{configStr});
    if (init) {
        return MugPluginSimple::add<Server>(config);
    }
    else {
        return MugPluginSimple::remomve<Server>(config);
    }
}

}

/**
 * Simplify the Bot interface for Mug server
 **/
#define THORS_ANVIL_SIMPLE_MUG_SERVER_INIT(Config, Server)                                      \
                                                                                                \
extern "C" ThorsAnvil::ThorsMug::MugPlugin* mugCreateInstance(int init, char const* configStr)  \
{                                                                                               \
    return ThorsAnvil::ThorsMug::mugCreateSimpleInstance<Server, Config>(init, configStr);      \
}

#endif
