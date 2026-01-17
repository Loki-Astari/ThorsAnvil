#ifndef THORSANVIL_SLACK_API_BLOCKACTIONS_H
#define THORSANVIL_SLACK_API_BLOCKACTIONS_H

#include "API.h"
#include "SlackBlockKit.h"
#include <string>
#include <map>
#include <vector>

namespace BK = ThorsAnvil::Slack::BlockKit;

// https://docs.slack.dev/reference/interaction-payloads/block_actions-payload
namespace ThorsAnvil::Slack::API
{

struct SlackUser
{
    std::string                         id;
    std::string                         username;
    std::string                         name;
    std::string                         team_id;
};

struct SlackContainer
{
    std::string                         type;
    std::string                         message_ts;
    std::string                         channel_id;
    bool                                is_ephemeral;
};

struct SlackTeam
{
    std::string                         id;
    std::string                         domain;
};

struct SlackEnterprise
{};
using PtrSlackEnterprise = SlackEnterprise*;
using OptPtrSlackEnterprise = std::optional<PtrSlackEnterprise>;

struct SlackChannel
{
    std::string                         id;
    std::string                         name;
};
using OptSlackChannel = std::optional<SlackChannel>;

struct SlackView
{};
using OptSlackView = std::optional<SlackView>;

// The value for a CheckBox
struct SlackSelectedCheckBox
{
    // std::string                         type;               // checkboxes
    BK::OptVecElOption                  selected_options;
    BK::OptVecElOption const&  getValue() const {return selected_options;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::SlackSelectedCheckBox, checkboxes);
    ThorsAnvil_TypeFieldName(type);
};

// The value for a text input field.
struct SlackPlainTextInput
{
    //std::string                         type;               // plain_text_input
    std::string                         value;

    std::string const&  getValue() const {return value;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::BlockKit::SlackPlainTextInput, plain_text_input);
    ThorsAnvil_TypeFieldName(type);
};

using InputValue = std::variant<SlackSelectedCheckBox, SlackPlainTextInput>;

// action_id of the "InputElement" object
// Note The "InputElement" is part of the "Input" object.
using SlackValue  = std::map<std::string, InputValue>;

// block_id of the "Input" object
using SlackValues = std::map<std::string, SlackValue>;

struct SlackState
{
    SlackValues                         values;
    template<typename T>
    typename T::ValueReturnType const& getValue(std::string const& block_id, std::string const& action_id) const
    {
        auto findBlock = values.find(block_id);
        if (findBlock != std::end(values)) {
            auto const& actionMap = findBlock->second;
            auto findAction = actionMap.find(action_id);
            if (findAction != std::end(actionMap)) {
                return std::get<typename T::ValueStorageType>(findAction->second).getValue();
            }
        }
        static typename T::ValueReturnType nullOption;
        return nullOption;
    }
};
using OptSlackState = std::optional<SlackState>;

struct SlackAction
{
    std::string                         type;               // "checkboxes", "datepicker", "datetimepicker", "radio_buttons", "static_select", "timepicker", "button", "overflow", plain_text_input

    std::string                         action_id;
    std::string                         block_id;
    std::string                         action_ts;
    // Value Part
    BK::OptString                       selected_date;      // ElActDatePicker      Example: "2025-12-25"
    BK::OptTime                         selected_date_time; // ElActDatetimePicker  Example: 1766732220
    BK::OptString                       selected_time;      // ElActTimePicker      Example: "04:00"
    BK::OptElText                       text;               // ElActButton
    BK::OptString                       value;              // ElActButton
    BK::OptVecElOption                  selected_options;   // ElActCheckbox
    BK::OptElOption                     selected_option;    // ElActRadioButton / ElActSelectMenu / ElActOverflowMenu
    // No Event Generated for:                              // ElActEMail / ElActNumberInput / ElActPlainTextInput / ElActURLInput
};
using VecSlackActions = std::vector<SlackAction>;
using OptVecSlackActions = std::optional<VecSlackActions>;

struct BlockActions
{
    // https://docs.slack.dev/reference/interaction-payloads/block_actions-payload
    // std::string                         type;               // Always "block_actions"
    std::string                         trigger_id;         // A short-lived ID that can be used to open modals.
    SlackUser                           user;               // The user who interacted to trigger this request.
    SlackTeam                           team;               // The workspace the app is installed on. Null if the app is org-installed.
    SlackContainer                      container;          // The container where this block action took place.
    std::string                         api_app_id;         // A string representing the app ID.
    OptPtrSlackEnterprise               enterprise;         // The enterprise the installed app is part of, if the app is either workspace-installed on an org, or org-installed.
    OptBool                             is_enterprise_install;
    OptSlackChannel                     channel;            // The channel where this block action took place.
    API::OptMessage                     message;            // The message where this block action took place, if the block was contained in a message.
    OptSlackView                        view;               // The view where this block action took place, if the block was contained in a view.
    OptVecSlackActions                  actions;            // Contains data from the specific interactive component that was used. App surfaces can contain blocks with multiple interactive components, and each of those components can have multiple values selected by users.
    OptSlackState                       state;              // A property including all stateful elements, not just input blocks
// Conditional Sent to non-function only
    std::string                         token;              // Represents a deprecated verification token feature. You should validate the request payload, however, and the best way to do so is to use the signing secret provided to your app.
    OptString                           response_url;       // (Deprecated for apps created with the Deno Slack SDK) A short-lived webhook that can be used to send messages in response to interactions.
// Conditional Sent to function only
    std::string                         hash;               // A unique value that is optionally accepted in views.update and views.publish API calls. When provided to those APIs, the hash is validated such that only the most recent view can be updated. This should be used to ensure the correct view is being updated when updates are happening asynchronously.
    std::string                         function_data;      // Metadata about the function execution that generated the block where this block action took place.
    std::string                         interactivity;      // An interactivity object generated as a result of the block action.
    std::string                         bot_access_token;   // A workflow (just-in-time) token generated for this block action.
// End of Conditional

    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::API::BlockActions, block_actions);
    ThorsAnvil_TypeFieldName(type);
};


}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackUser, id, username, name, team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackContainer, type, message_ts, channel_id, is_ephemeral);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackTeam, id, domain);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackEnterprise);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackChannel, id, name);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackView);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackSelectedCheckBox, selected_options);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackPlainTextInput, value);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackState, values);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::SlackAction, type, action_id, block_id, action_ts, selected_date, selected_date_time, selected_time, text, value, selected_options, selected_option);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::BlockActions, trigger_id, user, team, container, api_app_id, enterprise, is_enterprise_install, channel, message, view, actions, state, token, response_url, hash, function_data, interactivity, bot_access_token);

#endif

#if 0
{
    -"type": "block_actions",
    -"user": { "id": "U095XJHJ1J5", "username": "martin.york", "name": "martin.york", "team_id": "T095XJHH589" },
    -"api_app_id": "A09RQFXSEKC",
    -"token": "4axIIPlMUVqU5NWGL7OQOfO9",
    -"container": { "type": "message", "message_ts": "1766193241.611849", "channel_id": "C09RU2URYMS", "is_ephemeral": false },
    -"trigger_id": "10175204306689.9201629583281.42c5bd304cc3dca70074581f3103ea4f",
    -"team": { "id": "T095XJHH589", "domain": "thorsanvilworkspace" },
    -"enterprise": null,
    "is_enterprise_install": false,
    -"channel": { "id": "C09RU2URYMS", "name": "genreral" },
    -"message":
    {
        "type": "message",
        "user": "U09S3D8R00Z",
        "ts": "1766193241.611849",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "*Try and Insert* This is a test to get an insert 101, with interactive elements",
        "team": "T095XJHH589",
        "blocks":
        [
            { "type": "rich_text",
              "elements": [ { "type": "rich_text_section", "elements": [ { "type": "text", "text": "Try and Insert", "style": { "bold": true, "italic": false, "strike": false, "code": false } } ] } ],
              "block_id": "NNI7o"
            },
            { "type": "rich_text",
              "elements": [ { "type": "rich_text_section", "elements": [ { "type": "text", "text": "This is a test to get an insert", "style": { "bold": false, "italic": false, "strike": false, "code": false } } ] } ],
              "block_id": "0Eyu/"
            },
            { "type": "rich_text",
              "elements": [ { "type": "rich_text_section", "elements": [ { "type": "text", "text": "101", "style": { "bold": false, "italic": false, "strike": false, "code": false } } ] } ],
              "block_id": "H78Dl"
            },
            {
                "type": "input",
                "label": { "type": "plain_text", "text": "Options", "emoji": true },
                "dispatch_action": false,
                "optional": false,
                "element":
                {
                    "type": "checkboxes",
                    "action_id": "6943423a00427c3c55000878",
                    "options":
                    [
                        { "text": { "type": "plain_text", "text": "Done", "emoji": true }, "value": "Done" },
                        { "text": { "type": "plain_text", "text": "Urgent", "emoji": true }, "value": "Urgent" },
                        { "text": { "type": "plain_text", "text": "Important", "emoji": true }, "value": "Important" }
                    ],
                    "initial_options":
                    [
                        { "text": { "type": "plain_text", "text": "Important", "emoji": true }, "value": "Important" }
                    ]
                }
            },
            { "type": "divider" }
        ]
    },
    -"state":
    {
        "values":
        {
            "33yic":
            {
                "6943423a00427c3c55000878":
                {
                    "type": "checkboxes",
                    "selected_options":
                    [
                        { "text": { "type": "plain_text", "text": "Important", "emoji": true }, "value": "Important" },
                        { "text": { "type": "plain_text", "text": "Urgent", "emoji": true }, "value": "Urgent" }
                    ]
                }
            }
        }
    },
    -"response_url": "https://hooks.slack.com/actions/T095XJHH589/10165870246964/PHHkKFUrYO7BHUpAYavWpP0r",
    -"actions":
    // Depends on Input/Action Type.
    //
    // -- Input CheckBox: Only the now active ones are shown. Compare theis with the initial_options above to see the change.
    // -- Action CheckBox
    [
        {
            "type": "checkboxes",
            "block_id": "33yic",
            "action_id": "6943423a00427c3c55000878",
            "action_ts": "1766194729.658098",
            "selected_options":
            [
                { "text": { "type": "plain_text", "text": "Important", "emoji": true }, "value": "Important" },
                { "text": { "type": "plain_text", "text": "Urgent", "emoji": true }, "value": "Urgent" }
            ]
        }
    ]
    // -- Input: DatePicker:
    // -- Action: DatePicker:
    [
        {
            "type": "datepicker",
            "block_id": "4Yiyb",
            "action_id": "mERr+",
            "action_ts": "1766299774.551925",
            "selected_date": "2025-12-25"
        }
    ]
    // -- Input DataTime Picker
    // -- Action DataTime Picker
    [
        {
          "type": "datetimepicker",
          "block_id": "6zLYe",
          "action_id": "KC9gQ",
          "action_ts": "1766300250.881228",
          "selected_date_time": 1766732220
        }
    ]
    // -- Input Radio Button
    // -- Action Radio Button
    [
        {
            "type": "radio_buttons",
            "block_id": "Ym8Fe",
            "action_id": "xbzI5",
            "action_ts": "1766301998.142073",
            "selected_option":
            {
                "text":
                {
                    "type": "plain_text",
                    "text": "Done",
                    "emoji": true
                },
                "value": "Done"
            }
        }
    ]
    // -- Input Static Select
    // -- Action Static Select
    [
        {
            "type": "static_select",
            "block_id": "JZeMg",
            "action_id": "HeZXe",
            "action_ts": "1766302220.823687",
            "selected_option":
            {
                "text":
                {
                    "type": "plain_text",
                    "text": "Important",
                    "emoji": true
                },
                "value": "Important"
            }
        }
    ]
    // -- Input Time Picker
    // -- Action Time Picker
    [
        {
            "type": "timepicker",
            "block_id": "bn5Ee",
            "action_id": "8aBkl",
            "action_ts": "1766302426.329155"
            "selected_time": "04:00",
        }
    ]
    // -- Action: Button
    [
        {
          "type": "button",
          "block_id": "+2pUY",
          "action_id": "aQqei",
          "action_ts": "1766298664.946604",
          "value": "ButtonPressed",
          "text":
          {
            "type": "plain_text",
            "text": "Button",
            "emoji": true
          }
        }
    ]
    // -Action: Overflow Menu
    [
        {
            "type": "overflow",
            "block_id": "uaE8k",
            "action_id": "MyOverflow",
            "action_ts": "1766301218.876122",
            "selected_option":
            {
                "text":
                {
                    "type": "plain_text",
                    "text": "Important",
                    "emoji": true
                },
                "value": "Important"
            },
        }
    ]
}

#endif
