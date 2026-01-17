#ifndef THORSANVIL_SLACK_API_CHAT_SCHEDULE_MESSAGE_H
#define THORSANVIL_SLACK_API_CHAT_SCHEDULE_MESSAGE_H


#include "ThorsSlackConfig.h"
#include "APIChat.h"

namespace ThorsAnvil::Slack::API::Chat
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/chat.scheduleMessage
Response:
{
    "scheduled_message_id": "Q0A0H8V2U2V",
    "channel": "<Your Slack Channel>",
    "post_at": 1764215063,
    "message":
    {
        "user": "U09S3D8R00Z",
        "type": "message",
        "ts": "",
        "bot_id": "B09RJ4A000K",
        "app_id": "A09RQFXSEKC",
        "text": "I hope the tour went well, Mr. Wonka.",
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
                                "text": "I hope the tour went well, Mr. Wonka."
                            }
                        ]
                    }
                ]
            }
        ]
    },
    "ok": true
}
Documentation: https://docs.slack.dev/reference/methods/chat.deletescheduledmessage
Response:
{"ok":true}
Documentation: https://docs.slack.dev/reference/methods/chat.scheduledmessages.list
Response:
{
  "ok": true,
  "scheduled_messages":
  [
    {
      "id": "Q09UVPWRPP1",
      "channel_id": "<Your Slack Channel>",
      "post_at": 1764139506,
      "date_created": 1764139446,
      "text": "A timed message"
    },
    {
      "id": "Q0A00QD9K0U",
      "channel_id": "<Your Slack Channel>",
      "post_at": 1764139506,
      "date_created": 1764139446,
      "text": "A timed message"
    }
  ],
  "response_metadata":
  {
    "next_cursor": ""
  }
}
#endif

// Response Objects
struct ScheduledMessageReply
{
    bool                        ok      = false;
    std::string                 scheduled_message_id;
    std::string                 channel;
    std::time_t                 post_at;
    Message                     message;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::ScheduledMessageReply);
};

struct ScheduledMessagesItem
{
    std::string                 id;
    std::string                 channel_id;
    std::time_t                 post_at;
    std::time_t                 date_created;
    std::string                 text;
};
using VecScheduledMessagesItem = std::vector<ScheduledMessagesItem>;

struct ScheduledMessagesListReply
{
    bool                        ok      = false;
    VecScheduledMessagesItem    scheduled_messages;
    Cursor                      next_cursor;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::ScheduledMessagesListReply);
};


// Action Objects
struct ScheduleMessage
{
    static constexpr char const* api = "https://slack.com/api/chat.scheduleMessage";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = ScheduledMessageReply;

    std::string                 channel;                // Channel, private group, or DM channel to send message to. Can be an encoded ID, or a name. See below for more details.
    std::time_t                 post_at;                // Unix timestamp representing the future time the message should post to Slack.

    OptBool                     as_user;                // Set to true to post the message as the authed user, instead of as a bot. Defaults to false. Cannot be used by new Slack apps. See chat.postMessage.
    OptString                   attachments;            // A JSON-based array of structured attachments, presented as a URL-encoded string.
    BlockKit::OptBlocks         blocks;                 // A JSON-based array of structured blocks, presented as a URL-encoded string.
    OptBool                     link_names;             // Find and link user groups. No longer supports linking individual users; use syntax shown in Mentioning Users instead.
    OptString                   markdown_text;          // Accepts message text formatted in markdown. This argument should not be used in conjunction with blocks or text. Limit this field to 12,000 characters.
    OptString                   parse;                  // Change how messages are treated. See chat.postMessage.
    OptBool                     reply_broadcast;        // Used in conjunction with thread_ts and indicates whether reply should be made visible to everyone in the channel or conversation. Defaults to false.
    OptString                   text;                   // How this field works and whether it is required depends on other fields you use in your API call. See below for more detail.
    OptString                   thread_ts;              // Provide another message's ts value to make this message a reply. Avoid using a reply's ts value; use its parent instead.
    OptBool                     unfurl_links;           // Pass true to enable unfurling of primarily text-based content.
    OptBool                     unfurl_media;           // Pass false to disable unfurling of media content.
    OptMetadata                 metadata;               // JSON object with event_type and event_payload fields, presented as a URL-encoded string. Metadata you post to Slack is accessible to any app or user who is a member of that workspace.

};

struct DeleteScheduledMessage
{
    static constexpr char const* api = "https://slack.com/api/chat.deleteScheduledMessage";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    std::string             channel;                // The channel the scheduled_message is posting to
    std::string             scheduled_message_id;   // scheduled_message_id returned from call to chat.scheduleMessage
    OptBool                 as_user;                // Pass true to delete the message as the authed user with chat:write:user scope. Bot users in this context are considered authed users.
                                                    // If unused or false, the message will be deleted with chat:write:bot scope.
};

struct ScheduledMessagesList
{
    static constexpr char const* api = "https://slack.com/api/chat.scheduledMessages.list";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = ScheduledMessagesListReply;

    OptString       channel;            // The channel of the scheduled messages
    OptString       cursor;             // For pagination purposes, this is the cursor value returned from a previous call to chat.scheduledmessages.list indicating where you want to start this call from.
    OptString       latest;             // A Unix timestamp of the latest value in the time range
    OptInt          limit;              // Maximum number of original entries to return.
    OptString       oldest;             // A Unix timestamp of the oldest value in the time range
    OptString       team_id;            // encoded team id to list channels in, required if org token is used
};

}

// Response objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::ScheduledMessagesItem, id, channel_id, post_at, date_created, text);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::ScheduledMessageReply, ok, scheduled_message_id, channel, post_at, message);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::ScheduledMessagesListReply, ok, scheduled_messages, next_cursor);

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::ScheduleMessage, channel, post_at, as_user, attachments, blocks, link_names, markdown_text, parse, reply_broadcast, text, thread_ts, unfurl_links, unfurl_media, metadata);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::DeleteScheduledMessage, channel, scheduled_message_id, as_user);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::ScheduledMessagesList, channel, cursor, latest, limit, oldest, team_id);

#endif
