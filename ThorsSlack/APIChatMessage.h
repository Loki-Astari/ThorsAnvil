#ifndef THORSANVIL_SLACK_API_CHAT_POST_MESSAGE_H
#define THORSANVIL_SLACK_API_CHAT_POST_MESSAGE_H


#include "ThorsSlackConfig.h"
#include "APIChat.h"

namespace ThorsAnvil::Slack::API::Chat
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/chat.postmessage
Response:
{
    "ok": true,
    "channel": "<Your Slack Channel>",
    "ts": "1763354325.527129",
    "message":
    {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "1763354325.527129",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "I see: Time",
        "team": "T095XJHH589",
        "bot_profile":
        {
            "id": "B09RJ4A000K",
            "app_id": "A09RQFXSEKC",
            "user_id": "U09S3D8R00Z",
            "name": "ThorsSlackBotOne",
            "icons":
            {
                "image_36": "https://a.slack-edge.com/80588/img/plugins/app/bot_36.png",
                "image_48": "https://a.slack-edge.com/80588/img/plugins/app/bot_48.png",
                "image_72": "https://a.slack-edge.com/80588/img/plugins/app/service_72.png"
            },
            "deleted": false,
            "updated": 1762644664,
            "team_id": "T095XJHH589"
        },
        "blocks":
        [
            {
                "type": "rich_text",
                "block_id": "aRB4i",
                "elements":
                [
                    {
                        "type": "rich_text_section",
                        "elements":
                        [
                            {
                                "type": "text",
                                "text": "I see: Time"
                            }
                        ]
                    }
                ]
            }
        ]
    }
}
Documentation: https://docs.slack.dev/reference/methods/chat.postephemeral
Response:
{
    "ok":true,
    "message_ts":"1764217657.001000"
}
Documentation: https://docs.slack.dev/reference/methods/chat.delete
Response:
{
    "channel": "<Your Slack Channel>",
    "ts": "1764214726.831099",
    "ok": true
}
Documentation: https://docs.slack.dev/reference/methods/chat.update
Response:
{
    "channel": "<Your Slack Channel>",
    "ts": "1764215452.433709",
    "text": "Update text.",
    "message":
    {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "Update text.",
        "team": "T095XJHH589",
        "bot_profile":
        {
            "id": "B09RJ4A000K",
            "app_id": "A09RQFXSEKC",
            "user_id": "U09S3D8R00Z",
            "name": "ThorsSlackBotOne",
            "icons":
            {
                "image_36": "https://a.slack-edge.com/80588/img/plugins/app/bot_36.png",
                "image_48": "https://a.slack-edge.com/80588/img/plugins/app/bot_48.png",
                "image_72": "https://a.slack-edge.com/80588/img/plugins/app/service_72.png"
            },
            "deleted": false,
            "updated": 1762644664,
            "team_id": "T095XJHH589"
        },
        "blocks":
        [
            {
                "type": "rich_text",
                "elements":
                [
                    {
                        "type": "rich_text_section",
                        "elements":
                        [
                            {
                                "type": "text",
                                "text": "Update text."
                            }
                        ]
                    }
                ]
            }
        ]
    },
    "ok": true
}
#endif

// Response Objects
// Rant: For some reason windows defines a macro PostMessage
//       This causes untold issues. So to just avoid this
//       the name of this type breaks standard rules.
//       I apologise.
struct POSTMessageReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    Message                     message;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::POSTMessageReply);
};

struct PostEphemeralReply
{
    bool                        ok      = false;
    std::string                 message_ts;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::PostEphemeralReply);
};

struct DeleteReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::DeleteReply);
};

struct UpdateReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    std::string                 text;
    Message                     message;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::UpdateReply);
};

// Action Objects
// Rant: For some reason windows defines a macro PostMessage
//       This causes untold issues. So to just avoid this
//       the name of this type breaks standard rules.
//       I apologise.
struct POSTMessage
{
    static constexpr char const* api = "https://slack.com/api/chat.postMessage";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = POSTMessageReply;

    std::string                 channel;
    OptString                   text;
    BlockKit::OptBlocks         blocks;
    OptString                   icon_emoji;
    OptString                   username;
    OptString                   thread_ts;
};

struct PostEphemeral
{
    static constexpr char const* api = "https://slack.com/api/chat.postEphemeral";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = PostEphemeralReply;

    std::string                 channel;            // Channel, private group, or IM channel to send message to. Can be an encoded ID, or a name.
    std::string                 user;               // id of the user who will receive the ephemeral message. The user should be in the channel specified by the channel argument.
    OptBool                     as_user;            // (Legacy) Pass true to post the message as the authed user. Defaults to true if the chat:write:bot scope is not included. Otherwise, defaults to false.
    OptVecString                attachments;        // A JSON-based array of structured attachments, presented as a URL-encoded string.
    BlockKit::OptBlocks         blocks;             // A JSON-based array of structured blocks, presented as a URL-encoded string.
    OptString                   icon_emoji;         // Emoji to use as the icon for this message. Overrides icon_url.
    OptString                   icon_url;           // URL to an image to use as the icon for this message.
    OptBool                     link_names;         // Find and link channel names and usernames.
    OptString                   markdown_text;      // Accepts message text formatted in markdown. This argument should not be used in conjunction with blocks or text. Limit this field to 12,000 characters.
    OptString                   parse;              // Change how messages are treated. Defaults to none. See below.
    OptString                   text;               // How this field works and whether it is required depends on other fields you use in your API call. See below for more detail.
    OptString                   thread_ts;          // Provide another message's ts value to post this message in a thread. Avoid using a reply's ts value; use its parent's value instead. Ephemeral messages in threads are only shown if there is already an active thread.
    OptString                   username;           // Set your bot's user name.
};

struct Delete
{
    static constexpr char const* api = "https://slack.com/api/chat.delete";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = DeleteReply;

    std::string                 channel;        // Channel containing the message to be deleted.
    std::string                 ts;             // Timestamp of the message to be deleted.
    OptBool                     as_user;        // (Legacy) Pass true to delete the message as the authed user with chat:write:user scope. Bot users in this context are considered authed users. See legacy as_user parameter below.
};

struct Update
{
    static constexpr char const* api = "https://slack.com/api/chat.update";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = UpdateReply;

    std::string                 channel;        // Channel containing the message to be updated. For direct messages, ensure that this value is a DM ID (starts with D) instead of a User ID (starts with either U or W).
    std::string                 ts;             // Timestamp of the message to be updated.
    OptBool                     as_user;        // Pass true to update the message as the authed user. Bot users in this context are considered authed users.
    OptVecString                attachments;    // A JSON-based array of structured attachments, presented as a URL-encoded string.
                                            // TODO
                                            // Example: [{"pretext": "pre-hello", "text": "text-world"}]
    BlockKit::OptBlocks         blocks;         // A JSON-based array of structured blocks, presented as a URL-encoded string.
    OptString                   markdown_text;  // Accepts message text formatted in markdown. This argument should not be used in conjunction with blocks or text. Limit this field to 12,000 characters.
    OptMetadata                 metadata;       // JSON object with event_type and event_payload fields, presented as a URL-encoded string. If you don't include this field, the message's previous metadata will be retained. To remove previous metadata, include an empty object for this field. Metadata you post to Slack is accessible to any app or user who is a member of that workspace.
    OptBool                     link_names;     // Find and link channel names and usernames. Defaults to none. If you do not specify a value for this field, the original value set for the message will be overwritten with the default, none.
    OptBool                     parse;          // Change how messages are treated. Defaults to client, unlike chat.postMessage. Accepts either none or full. If you do not specify a value for this field, the original value set for the message will be overwritten with the default, client.
    OptString                   text;           // How this field works and whether it is required depends on other fields you use in your API call. See below for more detail.
    OptBool                     reply_broadcast;// Broadcast an existing thread reply to make it visible to everyone in the channel or conversation.
    OptVecString                file_ids;       // Array of new file ids that will be sent with this message.
};

}

// Response objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::POSTMessageReply, ok, channel, ts, message);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::PostEphemeralReply, ok, message_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::DeleteReply, ok, channel, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::UpdateReply, ok, channel, ts, text, message);

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::POSTMessage, channel, text, blocks, icon_emoji, username, thread_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::PostEphemeral, channel, user, attachments, blocks, icon_emoji, icon_url, link_names, markdown_text, parse, text, thread_ts, username);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::Delete, channel, ts, as_user);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::Update, channel, ts, as_user, attachments, blocks, markdown_text, metadata, link_names, parse, text, reply_broadcast, file_ids);

#endif
