#ifndef  THORSANVIL_NISSE_BOLT_APP_H
#define  THORSANVIL_NISSE_BOLT_APP_H

#include "NisseBoltConfig.h"
#include "AppConfig.h"
#include "Say.h"
#include "Ack.h"
#include "Response.h"
#include "Handlers.h"
#include "View.h"

#include "ThorsMug/MugPlugin.h"
#include "ThorsSlack/SlackClient.h"
#include "ThorsSlack/SlackEventHandler.h"

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <regex>

namespace ThorsAnvil::Nisse::Bolt
{
class App: ThorsAnvil::ThorsMug::MugPluginSimple
{
    std::string                                     slot;
    ThorsAnvil::Slack::EventHandlerMap              eventHandlerMap;
    ThorsAnvil::Slack::SlackClient                  client;
    ThorsAnvil::Slack::SlackEventHandler            slackHandler;
    ThorsAnvil::Slack::SlashCommandHandlerMap       slashCommandHandlerMap;
    ThorsAnvil::Slack::ActionHandlerMap             actionHandlerMap;
    ThorsAnvil::Slack::ViewHandlerMap               viewHandlerMap;

    std::vector<std::pair<Filter, MessageHandler>>  messageHandlers;
    std::vector<AnyEventHandler>                    eventHandlers;

        void addSlashCommandHandlers();
        void addUserActionHandlers();
        void addEventHandlers();
    public:
        App(AppConfig const& config);
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction() override;

        // register handlers for the standard Message events that come from the slack server.
        void message(Filter&& filter, MessageHandler&& handler);            // Generic filter.
        void message(std::string filter, MessageHandler&& handler);         // Simple to use string filter.
        void message(std::regex filter, MessageHandler&& handler);          // Simplish to use regular expression filter
        void message(MessageHandler&& handler);                             // No filter handle all events.

        // register handlers for other Events (other than Message) that come from the slack server.
        void event(EventHandler<Event::Message>&& handler)                  {message(std::move(handler));}
        template<typename E>
        void event(EventHandler<E>&& handler)                               {eventHandlers.emplace_back(std::move(handler));}

        // Handle slash commands:
        void command(std::string const& command, SlashCommandHandler&& handler);

        // Handle User Actions.
        void action(std::string const& actionId, ActionHandler&& handler);

        // Handle Views
        void viewOpen(std::string const& triggerId, View const& view);
        void viewPush(std::string const& triggerId, View const& view);
        void viewUpdate(std::string const& viewId, ThorsAnvil::Slack::API::Views::View view);

        // Temp
        ThorsAnvil::Slack::SlackClient const& getClient() const {return client;}
    private:
        // Handle incoming events and send to the registered handlers.
        template<typename T>
        void handleEvent(ThorsAnvil::Slack::EventRequest<T> const& request);
        void handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request);


        // Friend declaration so mugInterface can register robots.
        template<typename Server, typename ServerConfig>
        friend ThorsAnvil::ThorsMug::MugPlugin* mugCreateBoltInstance(int init, char const* configStr);

        // Addig and removing Bots at runtime.
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* add(ServerConfig&& serverConfig);
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* rem(ServerConfig&& serverConfig);
    private:
        // Currently active Bots.
        static std::map<std::string, std::unique_ptr<App>>& getServerInfo();
};


/**
 * Template functions that need to be in header file
 */
template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* App::add(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<App>>&    servers = getServerInfo();
    std::string const& slot = static_cast<AppConfig&>(serverConfig).slot;
    auto find = servers.find(slot);
    if (find != servers.end() && find->second.get() != nullptr) {
        // Extracting the pointer here
        // So I can use it in typeid() without generating a warning message.
        App* rawApp = find->second.get();
        ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::Bolt::App", "add", "Can not load Mug Server this slot >", slot, " is already being used. Current: ", typeid(*rawApp).name(), " New: ", typeid(Server).name());
    }
    find->second = std::make_unique<Server>(std::move(serverConfig));
    return find->second.get();
}

template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* App::rem(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<App>>&    servers = getServerInfo();
    auto find = servers.find(static_cast<AppConfig&>(serverConfig).slot);
    if (find != servers.end()) {
        find->second.reset();
    }
    return nullptr;
}

template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* mugCreateBoltInstance(int init, char const* configStr)
{
    ServerConfig    config = ThorsAnvil::Serialize::jsonBuilder<ServerConfig>(std::stringstream{configStr});
    if (init) {
        return ThorsAnvil::Nisse::Bolt::App::add<Server>(config);
    }
    else {
        return ThorsAnvil::Nisse::Bolt::App::rem<Server>(config);
    }
}

}

/**
 * Simplify the Bot interface for Mug server
 **/
#define THORS_ANVIL_NISSE_BOLT_SERVER_INIT(Config, Server)                                      \
                                                                                                \
extern "C" ThorsAnvil::ThorsMug::MugPlugin* mugCreateInstance(int init, char const* configStr)  \
{                                                                                               \
    return ThorsAnvil::Nisse::Bolt::mugCreateBoltInstance<Server, Config>(init, configStr);     \
}

#endif
