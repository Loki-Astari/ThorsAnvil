#ifndef THORSANVIL_SLACK_SLACKEVENTHANDLER_H
#define THORSANVIL_SLACK_SLACKEVENTHANDLER_H

#include "ThorsSlackConfig.h"
#include "EventAppMentioned.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorsSlackConfig.h"
#include "Client.h"
#include "APIViews.h"
#include "APIChatMessage.h"
#include "APIBlockActions.h"
#include "Event.h"
#include "EventCallback.h"
#include "SlashCommand.h"
#include "EventMessage.h"
#include "EventsReaction.h"
#include "EventURLVerification.h"
#include "Util.h"

#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
#include "ThorsCrypto/hmac.h"
#include "ThorSerialize/Logging.h"

#include <string>
#include <string_view>
#include <variant>
#include <algorithm>
#include <algorithm>
#include <cmath>
#include <ctime>

#if defined(DEBUG) && (DEBUG != 0)
#incude <iostream>
#endif

namespace ThorsAnvil::Slack
{

using ThorsAnvil::Nisse::HTTP::Request;
using ThorsAnvil::Nisse::HTTP::Response;

using EventObject = std::variant<API::BlockActions, API::Views::ViewSubmission, API::Views::ViewClosed>;
using CmdEvent  = std::variant<API::Views::ViewSubmission const*>;

template<typename T>
struct EventRequest
{
    ThorsAnvil::Nisse::HTTP::Request const&     request;
    ThorsAnvil::Nisse::HTTP::Response&          response;
    Event::EventCallback const&                 cb;
    T const&                                    event;
};
template<typename T>
using EventFunction      = std::function<void(EventRequest<T> const&)>;
using AnyEventFunction   = std::variant< EventFunction<Event::AppDeleted>, EventFunction<Event::AppHomeOpened>, EventFunction<Event::AppInstalled>, EventFunction<Event::AppRateLimited>, EventFunction<Event::AppRequested>, EventFunction<Event::AppUninstalledTeam>, EventFunction<Event::AppUninstalled>, EventFunction<Event::AppMentioned>,
                                        EventFunction<Event::AssistantThreadContextChanged>, EventFunction<Event::AssistantThreadStarted>,
                                        EventFunction<Event::CallRejected>,
                                        EventFunction<Event::ChannelArchive>, EventFunction<Event::ChannelCreated>, EventFunction<Event::ChannelDeleted>, EventFunction<Event::ChannelHistoryChanged>, EventFunction<Event::ChannelIdChanged>, EventFunction<Event::ChannelLeft>, EventFunction<Event::ChannelPostingPermissions>, EventFunction<Event::ChannelRename>, EventFunction<Event::ChannelShared>, EventFunction<Event::ChannelUnshared>,
                                        EventFunction<Event::DndUpdated>, EventFunction<Event::DndUpdatedUser>,
                                        EventFunction<Event::EmailDomainChanged>,
                                        EventFunction<Event::EmojiChanged>,
                                        EventFunction<Event::EntityDetailsRequested>,
                                        EventFunction<Event::FileChange>, EventFunction<Event::FileCommentAdded>, EventFunction<Event::FileCommentDeleted>, EventFunction<Event::FileCommentEdited>, EventFunction<Event::FileCreated>, EventFunction<Event::FileDeleted>, EventFunction<Event::FilePublic>, EventFunction<Event::FileShared>, EventFunction<Event::FileUnshared>,
                                        EventFunction<Event::FunctionExecuted>,
                                        EventFunction<Event::GridMigrationFinished>, EventFunction<Event::GridMigrationStarted>,
                                        EventFunction<Event::GroupClose>, EventFunction<Event::GroupDeleted>, EventFunction<Event::GroupHistoryChanged>, EventFunction<Event::GroupLeft>, EventFunction<Event::GroupOpen>, EventFunction<Event::GroupRename>,
                                        EventFunction<Event::ImClose>, EventFunction<Event::ImCreated>, EventFunction<Event::ImHistoryChanged>, EventFunction<Event::ImOpen>,
                                        EventFunction<Event::InviteRequested>,
                                        EventFunction<Event::LinkShared>,
                                        EventFunction<Event::MemberJoinedChannel>, EventFunction<Event::MemberLeftChannel>,
                                        EventFunction<Event::Message>,
                                        EventFunction<Event::MessageMetadataPosted>, EventFunction<Event::MessageMetadataUpdated>, EventFunction<Event::MessageMetadataDeleted>,
                                        EventFunction<Event::PinAdded>, EventFunction<Event::PinRemoved>,
                                        EventFunction<Event::ReactionAdded>, EventFunction<Event::ReactionRemoved>,
                                        EventFunction<Event::SharedChannelInviteAccepted>, EventFunction<Event::SharedChannelInviteApproved>, EventFunction<Event::SharedChannelInviteDeclined>, EventFunction<Event::SharedChannelInviteReceived>, EventFunction<Event::SharedChannelInviteRequested>,
                                        EventFunction<Event::StarAdded>, EventFunction<Event::StarRemoved>,
                                        EventFunction<Event::SubteamCreated>, EventFunction<Event::SubteamMembersChanged>, EventFunction<Event::SubteamSelfAdded>, EventFunction<Event::SubteamSelfRemoved>, EventFunction<Event::SubteamUpdated>,
                                        EventFunction<Event::TeamAccessGranted>, EventFunction<Event::TeamAccessRevoked>, EventFunction<Event::TeamDomainChange>, EventFunction<Event::TeamJoin>, EventFunction<Event::TeamRename>,
                                        EventFunction<Event::TokensRevoked>,
                                        EventFunction<Event::UserChange>, EventFunction<Event::UserConnection>, EventFunction<Event::UserHuddleChanged>
                                    >;
using EventFunctionMap   = std::map<std::string, AnyEventFunction>;

struct SlashCommandRequest
{
    ThorsAnvil::Nisse::HTTP::Request const&     request;
    ThorsAnvil::Nisse::HTTP::Response&          response;
    SlashCommand const&                         command;
};
using SlashCommandHandler    = std::function<void(SlashCommandRequest const&)>;
using SlashCommandHandlerMap = std::map<std::string, SlashCommandHandler>;

struct ActionHandlerRequest
{
    ThorsAnvil::Nisse::HTTP::Request const&     request;
    ThorsAnvil::Nisse::HTTP::Response&          response;
    ThorsAnvil::Slack::API::BlockActions const& command;
    std::string                                 value;
};
using ActionHandler         = std::function<void(ActionHandlerRequest const&)>;
using FilterHandler         = std::pair<std::string, ActionHandler>;
using ActionHandlerMap      = std::map<std::string, FilterHandler>;

struct ViewHandlerRequest
{
    ThorsAnvil::Nisse::HTTP::Request const&     request;
    ThorsAnvil::Nisse::HTTP::Response&          response;
    EventObject const&                          command;
};
using ViewSubmitHandler     = std::function<void(ThorsAnvil::Nisse::HTTP::Request const&, ThorsAnvil::Nisse::HTTP::Response&, API::Views::ViewSubmission const&)>;
using ViewClosedHandler     = std::function<void(ThorsAnvil::Nisse::HTTP::Request const&, ThorsAnvil::Nisse::HTTP::Response&, API::Views::ViewClosed const&)>;
using ViewActionHandler     = std::function<void(ThorsAnvil::Nisse::HTTP::Request const&, ThorsAnvil::Nisse::HTTP::Response&, API::BlockActions const&, std::string const& value)>;


struct View
{
    ViewSubmitHandler   submitHandler;
    ViewClosedHandler   closedHanlder;
    ActionHandlerMap    actionHandlerMap;
    std::string         parentView;

    void operator()(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, API::Views::ViewSubmission const& submit)                     const {submitHandler(request, response, submit);}
    void operator()(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, API::Views::ViewClosed const& close)                          const {closedHanlder(request, response, close);}
};

using ViewHandlerMap        = std::map<std::string, View>;

class EventHandler
{
        Client&         client;
        std::string     slackSecret;

        // For handling Events (all of them)
        EventFunctionMap const&         eventHandlerMap;

        // For handling Slash Commands
        SlashCommandHandlerMap const&   slashCommandHandlerMap;

        // For handling user Actions.
        ActionHandlerMap const&         actionHandlerMap;

        // For handling view Submissions.
        // This is not const as we remove handlers when they have been used.
        // Unlike the handlers above which are in the control of their owners.
        ViewHandlerMap&                 viewHandlerMap;

    public:
        EventHandler(Client& client, std::string_view slackSecret, EventFunctionMap const& eventHandlerMap, SlashCommandHandlerMap const& slashCommandHandlerMap, ActionHandlerMap const& actionHandlerMap, ViewHandlerMap& viewHandlerMap);

        // Method to validate Slack message comes from slack.
        bool validateRequest(Request const& request);

        // https://api.slack.com/apps/<Application ID>/event-subscriptions?
        // i.e. Event Subscriptions tab in your application configuration.
        void handleEvent(Request const& request, Response& response);
        // https://api.slack.com/apps/<Application ID>/interactive-messages?
        // i.e. Interactivity & Shortcuts tab in your application configuration.
        void handleUserActions(Request const& request, Response& response);
        // https://api.slack.com/apps/<Application ID>/slash-commands?
        // i.e. Slash command handlings
        void handleSlashCommand(Request const& request, Response& response);

    private:
        std::string getEventType(Request const& request, Response& response, bool& found);

        void handleURLVerificationEvent(Request const& request, Response& response, Event::EventURLVerification const& event);

        /* Local Visitor types */
        struct VisitorEvent
        {
            EventHandler&           plugin;
            Request const&          request;
            Response&               response;

            void operator()(Event::EventURLVerification const& event)    {plugin.handleURLVerificationEvent(request, response, event);}
            void operator()(Event::EventCallback const& event)           {std::visit(VisitorCallbackEvent{plugin, event, request, response}, event.event);}
        };
        struct VisitorCallbackEvent
        {
            EventHandler&               plugin;
            Event::EventCallback const& eventBase;
            Request const&              request;
            Response&                   response;

            template<typename T>
            void operator()(T const& event);
        };
        struct UserActionCallback
        {
            EventHandler&               plugin;
            Request const&              request;
            Response&                   response;

            // Handles View Interactions
            // API::Views::ViewSubmission, API::Views::ViewClosed
            template<typename T>
            void operator()(T const& viewAction);
            // Handles the interaction of individual components.
            void operator()(API::BlockActions const& userAction);
            private:
                void handleViewAction(API::Views::ViewSubmission const&, ViewHandlerMap::const_iterator)    {}
                void handleViewAction(API::Views::ViewClosed const&, ViewHandlerMap::const_iterator);

                // Need some extra work to find the change of state in a checkbox.
                std::string getCheckBoxValue(API::BlockActions const& userAction, API::SlackAction const& action);
                BlockKit::VecElOption& getInitialOptions(BlockKit::Block& block, std::string const& action_id);
        };

        // Simple Utility helpers.
        struct BlockIdGetter // Extract block_id from Block type
        {
            std::string const& operator()(BlockKit::Actions const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Context const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::ContextActions const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Divider const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::File const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Header const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Image const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Input const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Markdown const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::RichText const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Section const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Table const& block){return block.block_id.value();}
            std::string const& operator()(BlockKit::Video const& block){return block.block_id.value();}
        };
        struct ActionIdGetter // Extract block_id from ElActive type
        {
            std::string const& operator()(BlockKit::Button const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::Checkboxes const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::DatePicker const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::DateTimePicker const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::Overflow const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::RadioButtons const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::StaticSelect const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::TimePicker const& action){return action.action_id.value();}
        };

};

inline
EventHandler::EventHandler(Client& client, std::string_view slackSecret, EventFunctionMap const& eventHandlerMap, SlashCommandHandlerMap const& slashCommandHandlerMap, ActionHandlerMap const& actionHandlerMap, ViewHandlerMap& viewHandlerMap)
    : client{client}
    , slackSecret(slackSecret)
    , eventHandlerMap{eventHandlerMap}
    , slashCommandHandlerMap{slashCommandHandlerMap}
    , actionHandlerMap{actionHandlerMap}
    , viewHandlerMap{viewHandlerMap}
{}

inline
void EventHandler::handleEvent(Request const& request, Response& response)
{
    using ThorsAnvil::Nisse::HTTP::HeaderResponse;
    using namespace std::string_literals;
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "handleEvent", "Message Recieved: ", request);

    ThorsAnvil::Slack::Event::Event     event;
    bool found = false;
    ThorsAnvil::Serialize::ParserConfig config;
    config.setIdentifyDynamicClass([&](ThorsAnvil::Serialize::DataInputStream&){return getEventType(request, response, found);});
    request.body() >> ThorsAnvil::Serialize::jsonImporter(event, config);

    std::visit(VisitorEvent{*this, request, response}, event);
}

inline
bool EventHandler::validateRequest(Request const& request)
{
    std::string const&  sig = request.variables()["x-slack-signature"];
    std::string const&  timestampStr = request.variables()["x-slack-request-timestamp"];
    std::int32_t        timestamp;
    auto                first = &timestampStr[0];
    auto                last = first + timestampStr.size();
    auto                convertResult = std::from_chars(first, last, timestamp);
    auto                versionEnd = std::min(std::size(sig), sig.find('='));

    if ((convertResult.ec == std::errc::invalid_argument) || (convertResult.ptr != last) || std::abs(std::time(nullptr) - timestamp) > (60 * 5)) {
        return false;
    }

    using namespace std::literals::string_literals;
    using HMac = ThorsAnvil::Crypto::HMacBuilder<ThorsAnvil::Crypto::Sha256>;

    ThorsAnvil::Crypto::Digest<ThorsAnvil::Crypto::Sha256>      digest;
    {
        HMac hmac(slackSecret, digest);
        hmac.appendData(std::string_view{std::begin(sig), std::begin(sig) + versionEnd});
        hmac.appendData(":"s);
        hmac.appendData(timestampStr);
        hmac.appendData(":"s);

        std::string_view    body = request.preloadStreamIntoBuffer();
        hmac.appendData(body);
    }
    std::string exp{sig.c_str() + versionEnd + 1, sig.size() - versionEnd - 1};
    std::string dig = ThorsAnvil::Crypto::hexdigest<ThorsAnvil::Crypto::Sha256>(digest);
    bool result = (std::size(dig) == std::size(exp)) && (CRYPTO_memcmp(dig.c_str(), exp.data(), std::size(dig)) == 0);
    ThorsLogDebug("ThorsAnvil::Slack::EventHandler", "validateRequest", "Request Validation: ", (result ? "OK": "FAIL"));
    return result;
}

inline
std::string EventHandler::getEventType(Request const& request, Response& /*response*/, bool& found)
{
    if (!found) {
        found = true;
        std::string_view    body = request.preloadStreamIntoBuffer();
        if (body.find(R"("type":"url_verification")") != std::string_view::npos) {
            ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "getEventType", "Found: url_verification");
            return "url_verification";
        }
        if (body.find(R"("type":"event_callback")") != std::string_view::npos) {
            ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "getEventType", "Found: event_callback");
            return "event_callback";
        }
        ThorsLogError("ThorsAnvil::Slack::EventHandler", "getEventType", "Could not identify event type: ERROR");
    }
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "getEventType", "Found: Fallback object members");
    return "";
}


inline
void EventHandler::handleURLVerificationEvent(Request const& /*request*/, Response& response, Event::EventURLVerification const& event)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "handleURLVerificationEvent", "Sending URL Verification");
    ThorsAnvil::Nisse::HTTP::HeaderResponse  headers;
    headers.add("Content-Type", "application/json; charset=utf-8");

    ThorsAnvil::Slack::Event::ResponseURLVerification   reply{event.challenge};

    std::size_t         challangeBackSize = ThorsAnvil::Serialize::jsonStreanSize(reply);

    response.addHeaders(headers);
    response.body(challangeBackSize) << ThorsAnvil::Serialize::jsonExporter(reply, ThorsAnvil::Serialize::PrinterConfig{ThorsAnvil::Serialize::OutputType::Stream});
    return;
}

inline
void EventHandler::handleUserActions(Request const& request, Response& response)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "handleUserActions", "Recievent User Action");
    std::stringstream stream(request.variables()["payload"]);

    EventObject       eventObject;
    stream >> ThorsAnvil::Serialize::jsonImporter(eventObject);

    std::visit(UserActionCallback{*this, request, response}, eventObject);
}

inline
void EventHandler::handleSlashCommand(Request const& request, Response& response)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler", "handleSlashCommand", "Slash command: ");
    SlashCommand        command(request);
    auto find = slashCommandHandlerMap.find(command.command);
    if (find == slashCommandHandlerMap.end()) {
        ThorsLogError("ThorsAnvil::Slack::EventHandler", "handleSlashCommand", "Call to unimplemented command");
        response.setStatus(501);
        return;
    }
    find->second({request, response, command});
}

template<typename T>
void EventHandler::VisitorCallbackEvent::operator()(T const& event)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler::VisitorCallbackEvent", "operator()(event)", "Message Recieved:");
    std::string const& key = T::typeName();
    auto find = plugin.eventHandlerMap.find(key);
    if (find == plugin.eventHandlerMap.end()) {
        ThorsLogError("ThorsAnvil::Slack::EventHandler::VisitorCallbackEvent::operator()", key, "Call to unimplemented method");
        response.setStatus(501);
        return;
    }
    ThorsLogDebug("ThorsAnvil::Slack::EventHandler::VisitorCallbackEvent::operator()", key, "Calling client handler");
    AnyEventFunction const&  anyHandler   = find->second;
    EventFunction<T> const&  eventHandler = std::get<EventFunction<T>>(anyHandler);
    eventHandler(EventRequest<T>{request, response, eventBase, event});
}

template<typename T>
void EventHandler::UserActionCallback::operator()(T const& viewAction)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler::UserActionCallback", "operator()(ViewAction)", "Message Recieved:");
    std::string const&   viewId = viewAction.view.id;
    auto find = plugin.viewHandlerMap.find(viewId);
    if (find == plugin.viewHandlerMap.end()) {
        // No handler
        return;
    }
    View const& view = find->second;
    view(request, response, viewAction);
    handleViewAction(viewAction, find); // Used when closing multiple dialogs at the same time.
    plugin.viewHandlerMap.erase(find);
}

inline
void EventHandler::UserActionCallback::handleViewAction(API::Views::ViewClosed const& viewAction, ViewHandlerMap::const_iterator find)
{
    if (viewAction.is_cleared) {
        std::string parent = find->second.parentView;
        while (parent != "") {
            auto find = plugin.viewHandlerMap.find(parent);
            if (find == plugin.viewHandlerMap.end()) {
                break;
            }
            parent = find->second.parentView;
            plugin.viewHandlerMap.erase(find);
        }
    }
}

// Handles the interaction of individual components.
inline
void EventHandler::UserActionCallback::operator()(API::BlockActions const& userAction)
{
    ThorsLogTrack("ThorsAnvil::Slack::EventHandler::UserActionCallback", "operator()(BlockAction)", "Message Recieved:");
    auto view = plugin.viewHandlerMap.end();
    if (userAction.view.has_value()) {
        std::string const&          triggerId   = userAction.view.value().id;
        view = plugin.viewHandlerMap.find(triggerId);
    }
    ActionHandlerMap const& actionHandlerMap = view == plugin.viewHandlerMap.end() ? plugin.actionHandlerMap : view->second.actionHandlerMap;

    API::SlackAction const&     action      = userAction.actions.value()[0];
    std::string const&          actionId    = action.action_id;

    auto find = actionHandlerMap.find(actionId);
    if (find == actionHandlerMap.end()) {
        // No installed handler for this action.
        return;
    }

    std::string const&          filter      = find->second.first;
    if (!(filter == "" || filter == action.block_id)) {
        return;
    }

    std::string const&          type        = action.type;
    ActionHandler const&        handler     = find->second.second;
    InputValueToString          ptr2String;

    if (type == "datepicker") {
        handler({request, response, userAction, ptr2String(action.selected_date.value())});
    }
    else if (type == "datetimepicker") {
        handler({request, response, userAction, ptr2String(action.selected_date_time.value())});
    }
    else if (type == "timepicker") {
        handler({request, response, userAction, ptr2String(action.selected_time.value())});
    }
    else if (type == "checkboxes") {
        handler({request, response, userAction, getCheckBoxValue(userAction, action)});
    }
    else if (type == "radio_buttons") {
        handler({request, response, userAction, ptr2String(action.selected_option.value())});
    }
    else if (type == "static_select") {
        handler({request, response, userAction, ptr2String(action.selected_option.value())});
    }
    else if (type == "overflow") {
        handler({request, response, userAction, ptr2String(action.selected_option.value())});
    }
    else if (type == "button") {
        handler({request, response, userAction, ""});
    }
    else if (type == "plain_text_input") {
        handler({request, response, userAction, ptr2String(action.value.value())});
    }
    else {
        ThorsLogError("ThorsAnvil::Slack::EventHandler::UserActionCallback", "operator()", "Unknown Action: ", request.variables()["payload"]);
    }
}

inline
std::string EventHandler::UserActionCallback::getCheckBoxValue(API::BlockActions const& userAction, API::SlackAction const& action)
{
    std::unique_ptr<BlockKit::VecElOption> const& ptr2Value = action.selected_options.value();
    if (!ptr2Value) {
        ThorsLogError("ThorsAnvil::Slack::EventHandler::UserActionCallback", "getCheckBoxValue", "XX");
        return "";
    }
    BlockKit::VecElOption const& values = *ptr2Value;

    BK::Blocks blocks = userAction.view.has_value()
                                ? userAction.view.value().blocks
                                : userAction.message.value().blocks;

    // Make a copy of the blocks.
    // We are going to modify this with the new state and update the UI.
    // This will make sure that the new state matches what the user has clicked.

    // Extract the currently selected options into a set.
    std::set<std::string>   currentState;
    for (auto const& option: values) {
        currentState.insert(option.value);
    }

    // Extract the previously selected options
    // We need to search through the event to go find the exact info.
    std::set<std::string>   initState;
    std::string const& blockId = userAction.actions.value()[0].block_id;
    // We are searching fro the block that contains the set of checkboxes.
    for (auto& block : blocks) {
        std::string const& theBlockId = std::visit(BlockIdGetter{}, block);
        if (theBlockId == blockId) {
            // The initOptions is the value we need to update in the UI.
            // We need to find this to update it but also extract the initial state.
            BlockKit::VecElOption& initOption = getInitialOptions(block, userAction.actions.value()[0].action_id);

            // Get the initialy selected options into a set.
            for (auto const& option: initOption) {
                initState.insert(option.value);
            }
            // Updating the init options.
            // This will we will write back to the UI below.
            initOption = values;
        }
    }
    if (userAction.view.has_value()) {
        API::Views::ViewsInfo const& view = userAction.view.value();
        plugin.client.validateMessage(ThorsAnvil::Slack::API::Views::Update{.view = {
                                                                                     .title = view.title,
                                                                                     .blocks = blocks,
                                                                                     .close = *view.close,
                                                                                     .submit = *view.submit,
                                                                                     .private_metadata = view.private_metadata,
                                                                                     .callback_id = view.callback_id,
                                                                                     .clear_on_close = view.clear_on_close,
                                                                                     .notify_on_close = view.notify_on_close,
                                                                                     .external_id = view.external_id,
                                                                                     .submit_disabled = view.submit_disabled
                                                                                    },
                                                                            .view_id = userAction.view.value().id
                                                                           });
    }
    else {
        plugin.client.validateMessage(ThorsAnvil::Slack::API::Chat::Update{.channel = userAction.container.channel_id, .ts = userAction.container.message_ts, .blocks = blocks});
    }

    // Using the current state and the initial state.
    // We can now discover which checkbox the user clicked.
    // A lot of work thanks slack.
    std::set<std::string>   turnedOn;
    std::set<std::string>   turnedOff;

    std::set_difference(std::begin(currentState), std::end(currentState), std::begin(initState), std::end(initState),
                        std::inserter(turnedOn, turnedOn.begin()));
    std::set_difference(std::begin(initState), std::end(initState), std::begin(currentState), std::end(currentState),
                        std::inserter(turnedOff, turnedOff.begin()));


    // There should only be one change.
    if (turnedOn.size() + turnedOff.size() != 1) {
        ThorsLogError("ThorsAnvil::Slack::EventHandler::UserActionCallback", "getCheckBoxValue", "Change in state is not consistent: ", turnedOn.size(), " ", turnedOff.size());
    }
    else {
        if (turnedOn.size() == 1) {
            return std::string("+") + (*turnedOn.begin());
        }
        if (turnedOff.size() == 1) {
            return std::string("-") + (*turnedOff.begin());
        }
    }
    return "";
}

inline
BlockKit::VecElOption&  EventHandler::UserActionCallback::getInitialOptions(BlockKit::Block& block, std::string const& action_id)
{
    static BlockKit::VecElOption nullOption;
    nullOption.clear();

    if (holds_alternative<BlockKit::Input>(block)) {
        BlockKit::Input& inputBlock = std::get<BlockKit::Input>(block);
        BlockKit::Checkboxes& inputCheckBox = std::get<BlockKit::Checkboxes>(inputBlock.element);
        if (!inputCheckBox.initial_options.has_value()) {
            inputCheckBox.initial_options = BlockKit::VecElOption{};
        }
        return inputCheckBox.initial_options.value();
    }
    else if (holds_alternative<BlockKit::Actions>(block)) {
        BlockKit::Actions& actionsBlock = std::get<BlockKit::Actions>(block);
        std::vector<BlockKit::ElActive>& actionBlockElements = actionsBlock.elements;
        for (auto& element: actionBlockElements) {
            std::string const& actionId = std::visit(ActionIdGetter{}, element);
            if (action_id == actionId) {
                BlockKit::Checkboxes& inputCheckBox = std::get<BlockKit::Checkboxes>(element);
                if (!inputCheckBox.initial_options.has_value()) {
                    inputCheckBox.initial_options = BlockKit::VecElOption{};
                }
                return inputCheckBox.initial_options.value();
            }
        }
    }
    return nullOption;
}

}

#endif
