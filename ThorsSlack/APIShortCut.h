#ifndef THORSANVIL_SLACK_API_SHORT_CUT_H
#define THORSANVIL_SLACK_API_SHORT_CUT_H

#include "ThorsSlackConfig.h"
#include "APIBlockActionsState.h"
#include "API.h"
#include <string>
#include <NisseHTTP/Request.h>

namespace ThorsAnvil::Slack::API
{

/*
 * This object is constructed from application/x-www-form-urlencoded encoded parameters.
 * {
 *    "type": "message_action",
 *    "token": "iOD4LRMJ4zhLOHMjSgQ26dpA",
 *    "action_ts": "1782083965.639429",
 *    "team": {
 *      "id": "T095XJHH589",
 *      "domain": "thorsanvilworkspace"
 *    },
 *    "user": {
 *      "id": "U095XJHJ1J5",
 *      "username": "martin.york",
 *      "team_id": "T095XJHH589",
 *      "name": "martin.york"
 *    },
 *    "channel": {
 *      "id": "C09RU2URYMS",
 *      "name": "genreral"
 *    },
 *    "is_enterprise_install": false,
 *    "enterprise": null,
 *    "callback_id": "CreateToDo",
 *    "trigger_id": "11415815055812.9201629583281.37afa334ab57a49db3b2aa5a6fae763d",
 *    "response_url": "https://hooks.slack.com/app/T095XJHH589/11408780887669/Fo6dxykkGiGuo4RTm33XF1R1",
 *    "message_ts": "1782071501.075519",
 *    "message": {
 *      "user": "U0A6EA93KEE",
 *      "type": "message",
 *      "ts": "1782071501.075519",
 *      "bot_id": "B0A6AT5U23F",
 *      "app_id": "A0A6T8BJ7C1",
 *      "text": "I hope the tour went well, Mr. Wonka.",
 *      "team": "T095XJHH589",
 *      "bot_profile": {
 *        "id": "B0A6AT5U23F",
 *        "deleted": false,
 *        "name": "ThorsToDo",
 *        "updated": 1767315049,
 *        "app_id": "A0A6T8BJ7C1",
 *        "user_id": "U0A6EA93KEE",
 *        "icons": {
 *          "image_36": "https://a.slack-edge.com/80588/img/plugins/app/bot_36.png",
 *          "image_48": "https://a.slack-edge.com/80588/img/plugins/app/bot_48.png",
 *          "image_72": "https://a.slack-edge.com/80588/img/plugins/app/service_72.png"
 *        },
 *        "team_id": "T095XJHH589"
 *      },
 *      "blocks": [
 *        {
 *          "type": "rich_text",
 *          "block_id": "ikpG",
 *          "elements": [
 *            {
 *              "type": "rich_text_section",
 *              "elements": [
 *                {
 *                  "type": "text",
 *                  "text": "I hope the tour went well, Mr. Wonka."
 *                }
 *              ]
 *            }
 *          ]
 *        }
 *      ]
 *    }
 *  }
 *
 */
struct ShortCutMessage
{
    // https://docs.slack.dev/reference/interaction-payloads/shortcuts-interaction-payload/
    std::string             callback_id;    // An ID that you defined when creating the shortcut.
    std::string             trigger_id;     // A temporary ID generated for the interaction in your app. This value can be used to open modals.
    std::string             response_url;   // A temporary webhook that can be used to send messages in response to interactions. This field will only be included for message shortcuts. If you want to send messages in response to global shortcuts, read this guide.
    SlackUser               user;           // The user who initiated the shortcut.
    Event::Message          message;        // The message that the user initiated the shortcut from. This field will only be included for message shortcuts. This will include the full structure of the message.
    SlackChannel            channel;        // The channel that the source message was located in. This field will only be included for message shortcuts.
    SlackTeam               team;           // The workspace (what we used to call a team) that the shortcut was initiated in.
    std::string             token;          // A deprecated verification token feature. You should validate the request payload, however, and the best way to do so is to use the signing secret provided to your app.

    // Suspect these fields will be there.
    // Further testing required.
    std::string             action_ts;
    OptBool                 is_enterprise_install;
    OptPtrSlackEnterprise   enterprise;
    std::string             message_ts;
};


struct ShortCut: public ShortCutMessage
{
    // std::string             type;           // Helps identify which type of interactive component sent the payload. Global shortcuts will return shortcut, message shortcuts will return message_action.
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::API::ShortCut, shortcut);
    ThorsAnvil_TypeFieldName(type);
};

// Very Similar to BlockActions
struct MessageAction: public ShortCutMessage
{
    // std::string             type;           // "message_action",
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::API::MessageAction, message_action);
    ThorsAnvil_TypeFieldName(type);
};


}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::ShortCutMessage, token, action_ts, team, user, channel, is_enterprise_install, enterprise, callback_id, trigger_id, response_url, message_ts, message);
ThorsAnvil_ExpandTrait(ThorsAnvil::Slack::API::ShortCutMessage, ThorsAnvil::Slack::API::ShortCut);
ThorsAnvil_ExpandTrait(ThorsAnvil::Slack::API::ShortCutMessage, ThorsAnvil::Slack::API::MessageAction);

#endif
