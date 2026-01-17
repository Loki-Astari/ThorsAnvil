#ifndef THORSANVIL_SLACK_SLACKEVENTHANDLER_H
#define THORSANVIL_SLACK_SLACKEVENTHANDLER_H

#include "EventCallbackAppMentioned.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorsSlackConfig.h"
#include "APIBlockActions.h"
#include "APIViews.h"
#include "Event.h"
#include "EventCallback.h"
#include "SlashCommand.h"
#include "EventCallbackMessage.h"
#include "EventCallbackReaction.h"
#include "EventURLVerification.h"

#include "ThorsMug/MugPlugin.h"
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

using EventObject = std::variant<API::BlockActions, API::Views::ViewSubmission>;
using CmdEvent  = std::variant<SlashCommand const*, Event::EventCallback const*, API::Views::ViewSubmission const*>;

struct SlackRequest
{
    ThorsAnvil::Nisse::HTTP::Request const&   request;
    ThorsAnvil::Nisse::HTTP::Response&        response;
    CmdEvent const&                           event;
};

class SlackEventHandler
{
        using Cmd       = std::function<void(SlackRequest const&)>;
        using CmdMap    = std::map<std::string, Cmd>;

        std::string     slackSecret;
        CmdMap          cmdMap;
    public:
        SlackEventHandler(std::string_view slackSecret, CmdMap&& cmdMap = {});

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

        void handleCallbackEvent(Request const& request, Response& response, Event::EventCallback const& event);

        /*
         * The following 7 methods are called from: handleCallbackEvent Which is called from handleEvent.
         */
        virtual void handleCallbackMessageEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::Message const&)                       { handleUsingCmdMap(request, response, &eventBase, "Event/Message", "handleCallbackMessageEvent");}
        virtual void handleCallbackReactionAddedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::ReactionAdded const& event)     { handleUsingCmdMap(request, response, &eventBase, "Event/ReactionAdded/" + event.reaction, "handleCallbackReactionAddedEvent");}
        virtual void handleCallbackReactionRemovedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::ReactionRemoved const& event) { handleUsingCmdMap(request, response, &eventBase, "Event/ReactionRemoved/" + event.reaction, "handleCallbackReactionRemovedEvent");}
        virtual void handleCallbackPinAddedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::PinAdded const&)                     { handleUsingCmdMap(request, response, &eventBase, "Event/PinAdded", "handleCallbackPinAddedEvent");}
        virtual void handleCallbackPinRemovedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::PinRemoved const&)                 { handleUsingCmdMap(request, response, &eventBase, "Event/PinRemoved", "handleCallbackPinRemovedEvent");}
        virtual void handleCallbackStarAddedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::StarAdded const&)                   { handleUsingCmdMap(request, response, &eventBase, "Event/StarAdded", "handleCallbackStarAddedEvent");}
        virtual void handleCallbackStarRemovedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::StarRemoved const&)               { handleUsingCmdMap(request, response, &eventBase, "Event/StarRemoved", "handleCallbackStarRemovedEvent");}
        virtual void handleCallbackAppMentionedEvent(Request const& request, Response& response, Event::EventCallback const& eventBase, Event::AppMentioned const&)             { handleUsingCmdMap(request, response, &eventBase, "Event/AppMentioned", "handleCallbackAppMentionedEvent");}
        virtual void handleActionsViewSubmit(Request const& request, Response& response, API::Views::ViewSubmission const& event)                                               { handleUsingCmdMap(request, response, &event, "View/ViewSubmission/" + event.view.id, "handleActionsViewSubmit"); }

        /*
         * The following 8 methods are called from: handleUserActions
         */
        virtual void handleActionsDatePicker(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*selected_date*/)    { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsDatePicker", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsDateTimePicker(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::time_t /*selected_date_time*/)  { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsDateTimePicker", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsTimePicker(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*selected_time*/)    { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsTimePicker", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsRadioButton(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*value*/)           { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsRadioButton", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsStaticMenu(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*value*/)            { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsStaticMenu", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsOverflowMenu(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*value*/)          { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsOverflowMenu", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsButton(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*value*/)                { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsButton", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsPlainTextInput(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, std::string const& /*value*/)        { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "handleActionsPlainTextInput", "Call to unimplemented method"); response.setStatus(501); }
        virtual void handleActionsCheckBox(Request const& /*request*/, Response& response, API::BlockActions const& event, std::string const& action_id, BlockKit::VecElOption const& value);


        /*
         * The following methods is called from: handleSlashCommand
         */
        virtual void handleSlashWithCommand(Request const& request, Response& response, SlashCommand const& command)      {handleUsingCmdMap(request, response, &command, command.command + "/" + command.text, "handleSlashWithCommand");}

        /*
         * Checkboxes are complicated.
         * Need some extra processing before handing of
         */
        virtual void handleActionsCheckBox(Request const&, Response& response, API::BlockActions const&, std::string const& /*action_id*/, BlockKit::VecElOption const& /*value*/, std::string const& /*changed*/, bool /*state*/, BlockKit::Blocks& /*newUI*/)
        { ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "SlackEventHandler", "Call to unimplemented method"); response.setStatus(501); }
        BlockKit::VecElOption&  getInitialOptions(BlockKit::Block& block, std::string const& action_id);

        void handleUsingCmdMap(Request const& request, Response& response, CmdEvent const& event, std::string const& key, char const* msg);

        /* Local Visitor types */
        struct VisitorEvent
        {
            SlackEventHandler&      plugin;
            Request const&          request;
            Response&               response;

            void operator()(Event::EventURLVerification const& event)    {plugin.handleURLVerificationEvent(request, response, event);}
            void operator()(Event::EventCallback const& event)           {plugin.handleCallbackEvent(request, response, event);}
        };
        struct VisitorCallbackEvent
        {
            SlackEventHandler&          plugin;
            Event::EventCallback const& eventBase;
            Request const&              request;
            Response&                   response;

            void operator()(Event::Message const& event)                 {plugin.handleCallbackMessageEvent(request, response, eventBase, event);}
            void operator()(Event::ReactionAdded const& event)           {plugin.handleCallbackReactionAddedEvent(request, response, eventBase, event);}
            void operator()(Event::ReactionRemoved const& event)         {plugin.handleCallbackReactionRemovedEvent(request, response, eventBase, event);}
            void operator()(Event::PinAdded const& event)                {plugin.handleCallbackPinAddedEvent(request, response, eventBase, event);}
            void operator()(Event::PinRemoved const& event)              {plugin.handleCallbackPinRemovedEvent(request, response, eventBase, event);}
            void operator()(Event::StarAdded const& event)               {plugin.handleCallbackStarAddedEvent(request, response, eventBase, event);}
            void operator()(Event::StarRemoved const& event)             {plugin.handleCallbackStarRemovedEvent(request, response, eventBase, event);}
            void operator()(Event::AppMentioned const& event)            {plugin.handleCallbackAppMentionedEvent(request, response, eventBase, event);}
        };
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
            std::string const& operator()(BlockKit::ElActButton const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActCheckbox const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActDatePicker const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActDatetimePicker const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActOverflowMenu const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActRadioButton const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActSelectMenu const& action){return action.action_id.value();}
            std::string const& operator()(BlockKit::ElActTimePicker const& action){return action.action_id.value();}
        };

};

inline
SlackEventHandler::SlackEventHandler(std::string_view slackSecret, CmdMap&& cmdMap)
    : slackSecret(slackSecret)
    , cmdMap{std::move(cmdMap)}
{}

inline
void SlackEventHandler::handleEvent(Request const& request, Response& response)
{
    using ThorsAnvil::Nisse::HTTP::HeaderResponse;
    using namespace std::string_literals;
    ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "handleEvent", "Message Recieved: ", request);

    ThorsAnvil::Slack::Event::Event     event;
    bool found = false;
    ThorsAnvil::Serialize::ParserConfig config;
    config.setIdentifyDynamicClass([&](ThorsAnvil::Serialize::DataInputStream&){return getEventType(request, response, found);});
    request.body() >> ThorsAnvil::Serialize::jsonImporter(event, config);

    std::visit(VisitorEvent{*this, request, response}, event);
}

inline
bool SlackEventHandler::validateRequest(Request const& request)
{
#if 0
    /*
     * TODO:
     * =====
     * Validate that the request is timely.
     * and that it is correctly signed.
     * See: https://docs.slack.dev/authentication/verifying-requests-from-slack
     */
#endif
    std::string const&  sig = request.variables()["x-slack-signature"];
    std::string const&  timestampStr = request.variables()["x-slack-request-timestamp"];
    auto                timestamp = std::stoll(timestampStr);
    auto                versionEnd = std::min(std::size(sig), sig.find('='));

    if (std::abs(std::time(nullptr) - timestamp) > (60 * 5)) {
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
    std::size_t versionNext = versionEnd + (versionEnd == std::size(sig) ? 0 : 1);
    bool result = (ThorsAnvil::Crypto::hexdigest<ThorsAnvil::Crypto::Sha256>(digest) == std::string_view{std::begin(sig) + versionNext, std::end(sig)});
    ThorsLogDebug("ThorsAnvil::Slack::SlackEventHandler", "validateRequest", "Request Validation: ", (result ? "OK": "FAIL"));
    return result;
}

inline
std::string SlackEventHandler::getEventType(Request const& request, Response& /*response*/, bool& found)
{
    if (!found) {
        found = true;
        std::string_view    body = request.preloadStreamIntoBuffer();
        if (body.find(R"("type":"url_verification")") != std::string_view::npos) {
            ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "getEventType", "Found: url_verification");
            return "url_verification";
        }
        if (body.find(R"("type":"event_callback")") != std::string_view::npos) {
            ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "getEventType", "Found: event_callback");
            return "event_callback";
        }
        ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", "getEventType", "Could not identify event type: ERROR");
    }
    ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "getEventType", "Found: Fallback object members");
    return "";
}


inline
void SlackEventHandler::handleURLVerificationEvent(Request const& /*request*/, Response& response, Event::EventURLVerification const& event)
{
    ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "handleURLVerificationEvent", "Sending URL Verification");
    ThorsAnvil::Nisse::HTTP::HeaderResponse  headers;
    headers.add("Content-Type", "application/json; charset=utf-8");

    ThorsAnvil::Slack::Event::ResponseURLVerification   reply{event.challenge};

    std::size_t         challangeBackSize = ThorsAnvil::Serialize::jsonStreanSize(reply);

    response.addHeaders(headers);
    response.body(challangeBackSize) << ThorsAnvil::Serialize::jsonExporter(reply, ThorsAnvil::Serialize::PrinterConfig{ThorsAnvil::Serialize::OutputType::Stream});
    return;
}

inline
void SlackEventHandler::handleCallbackEvent(Request const& request, Response& response, Event::EventCallback const& event)
{
    ThorsLogTrack("ThorsAnvil::Slack::SlackEventHandler", "handleCallbackEvent", "Handling callback event");
    std::visit(VisitorCallbackEvent{*this, event, request, response}, event.event);
}

inline
void SlackEventHandler::handleUserActions(Request const& request, Response& response)
{
    ThorsLogTrack("ThorsAnvil::Slack::TodoSlackEventHandler", "handleUserActions", "Recievent User Action");
    std::stringstream stream(request.variables()["payload"]);

    EventObject       eventObject;
    stream >> ThorsAnvil::Serialize::jsonImporter(eventObject);

    if (std::holds_alternative<API::BlockActions>(eventObject)) {
        API::BlockActions const&    event  = std::get<API::BlockActions>(eventObject);
        API::SlackAction const&     action = event.actions.value()[0];
        std::string const&          type   = action.type;

        if (type == "datepicker") {
            handleActionsDatePicker(request, response, event, action.action_id, action.selected_date.value());
        }
        else if (type == "datetimepicker") {
            handleActionsDateTimePicker(request, response, event, action.action_id, action.selected_date_time.value());
        }
        else if (type == "timepicker") {
            handleActionsTimePicker(request, response, event, action.action_id, action.selected_time.value());
        }
        else if (type == "checkboxes") {
            handleActionsCheckBox(request, response, event, action.action_id, action.selected_options.value());
        }
        else if (type == "radio_buttons") {
            handleActionsRadioButton(request, response, event, action.action_id, action.selected_option->value);
        }
        else if (type == "static_select") {
            handleActionsStaticMenu(request, response, event, action.action_id, action.selected_option->value);
        }
        else if (type == "overflow") {
            handleActionsOverflowMenu(request, response, event, action.action_id, action.selected_option->value);
        }
        else if (type == "button") {
            handleActionsButton(request, response, event, action.action_id, action.value.value());
        }
        else if (type == "plain_text_input") {
            handleActionsPlainTextInput(request, response, event, action.action_id, action.value.value());
        }
        else {
            ThorsLogError("UserTodoSlackEventHandler", "handleUserActions", "Unknown Action: ", request.variables()["payload"]);
        }
    }
    else if (std::holds_alternative<API::Views::ViewSubmission>(eventObject)) {
        API::Views::ViewSubmission const&    event  = std::get<API::Views::ViewSubmission>(eventObject);
        handleActionsViewSubmit(request, response, event);
    }
}

inline
BlockKit::VecElOption&  SlackEventHandler::getInitialOptions(BlockKit::Block& block, std::string const& action_id)
{
    static BlockKit::VecElOption nullOption;
    nullOption.clear();

    if (holds_alternative<BlockKit::Input>(block)) {
        BlockKit::Input& inputBlock = std::get<BlockKit::Input>(block);
        BlockKit::ElActCheckbox& inputCheckBox = std::get<BlockKit::ElActCheckbox>(inputBlock.element);
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
                BlockKit::ElActCheckbox& inputCheckBox = std::get<BlockKit::ElActCheckbox>(element);
                if (!inputCheckBox.initial_options.has_value()) {
                    inputCheckBox.initial_options = BlockKit::VecElOption{};
                }
                return inputCheckBox.initial_options.value();
            }
        }
    }
    return nullOption;
}

inline
void SlackEventHandler::handleActionsCheckBox(Request const& request, Response& response, API::BlockActions const& event, std::string const& action_id, BlockKit::VecElOption const& values)
{
    ThorsLogDebug("SlackEventHandler", "processesActionsCheckBox", "Recievent User Click on Checkbox");

    // Make a copy of the blocks.
    // We are going to modify this with the new state and update the UI.
    // This will make sure that the new state matches what the user has clicked.
    BlockKit::Blocks blocks = event.message.value().blocks;

    // Extract the currently selected options into a set.
    std::set<std::string>   currentState;
    for (auto const& option: values) {
        currentState.insert(option.value);
    }

    // Extract the previously selected options
    // We need to search through the event to go find the exact info.
    std::set<std::string>   initState;
    std::string const& blockId = event.actions.value()[0].block_id;
    // We are searching fro the block that contains the set of checkboxes.
    for (auto& block : blocks) {
        std::string const& theBlockId = std::visit(BlockIdGetter{}, block);
        if (theBlockId == blockId) {
            // The initOptions is the value we need to update in the UI.
            // We need to find this to update it but also extract the initial state.
            BlockKit::VecElOption&  initOption = getInitialOptions(block, action_id);

            // Get the initialy selected options into a set.
            for (auto const& option: initOption) {
                initState.insert(option.value);
            }
            // Updating the init options.
            // This will we will write back to the UI below.
            initOption = values;
        }
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
        ThorsLogError("TodoSlackEventHandler", "handleActionsCheckBox", "Change in state is not consistent: ", turnedOn.size(), " ", turnedOff.size());
    }
    else {
        if (turnedOn.size() == 1) {
            handleActionsCheckBox(request, response, event, action_id, values, (*turnedOn.begin()), true, blocks);
        }
        if (turnedOff.size() == 1) {
            handleActionsCheckBox(request, response, event, action_id, values, (*turnedOff.begin()), false, blocks);
        }
    }

}

inline
void SlackEventHandler::handleSlashCommand(Request const& request, Response& response)
{
    SlashCommand        command(request);
    handleSlashWithCommand(request, response, command);
}

inline
void SlackEventHandler::handleUsingCmdMap(Request const& request, Response& response, CmdEvent const& event, std::string const& key, char const* msg)
{
    auto find = cmdMap.find(key);
    if (find == cmdMap.end()) {
        ThorsLogError("ThorsAnvil::Slack::SlackEventHandler", msg, "Call to unimplemented method");
        response.setStatus(501);
        return;
    }
    ThorsLogDebug("ThorsAnvil::Slack::SlackEventHandler", msg, "Calling client handler");
    find->second(SlackRequest{request, response, event});
}

}

#endif
