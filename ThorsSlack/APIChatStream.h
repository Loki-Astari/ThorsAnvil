#ifndef THORSANVIL_SLACK_API_CHAT_STREAM_H
#define THORSANVIL_SLACK_API_CHAT_STREAM_H


#include "ThorsSlackConfig.h"
#include "APIChat.h"

namespace ThorsAnvil::Slack::API::Chat
{

#if 0
Documentation: https://docs.slack.dev/reference/methods/chat.appendStream
TODO. Needs payed account to test and validate.
Expected-Response:
{
  "ok": true,
  "channel": "C123ABC456",
  "ts": "1503435956.000247"
}
Documentation: https://docs.slack.dev/reference/methods/chat.startstream
TODO. Needs payed account to test and validate.
Expected-Response:
{
  "ok": true,
  "channel": "C123ABC456",
  "ts": "1503435956.000247"
}
Documentation: https://docs.slack.dev/reference/methods/chat.stopStream
TODO. Needs payed account to test and validate.
Expected-Response:
{
  "ok": true,
  "channel": "C123ABC456",
  "ts": "1503435956.000247",
  "message":
  {
    "text": "Here's the final streamed message content",
    "bot_id": "B123ABC456",
    "ts": "1503435956.000247",
    "type": "message",
    "subtype": "bot_message"
  }
}
#endif

// Response Objects
struct AppendStreamReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::AppentStreamReply);
};

struct StartStreamReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::StartStreamReply);
};

struct StopStreamReply
{
    bool                        ok      = false;
    std::string                 channel;
    std::string                 ts;
    Message                     message;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Chat::StopStreamReply);
};


// Action Objects
struct AppendStream
{
    static constexpr char const* api = "https://slack.com/api/chat.appendStream";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = AppendStreamReply;

    std::string                 channel;            // An encoded ID that represents a channel, private group, or DM
    std::time_t                 ts;                 // The timestamp of the streaming message.
    OptString                   markdown_text;      // Accepts message text formatted in markdown. Limit this field to 12,000 characters.
};

struct StartStream
{
    static constexpr char const* api = "https://slack.com/api/chat.startStream";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    std::string                 channel;            // An encoded ID that represents a channel thread or DM.
    std::string                 thread_ts;          // Provide another message's ts value to reply to. Streamed messages should always be replies to a user request.
    OptString                   markdown_text;      // Accepts message text formatted in markdown. Limit this field to 12,000 characters.
    OptString                   recipient_user_id;  // The encoded ID of the user to receive the streaming text. Required when streaming to channels.
    OptString                   recipient_team_id;  // The encoded ID of the team the user receiving the streaming text belongs to. Required when streaming to channels.
};

struct StopStream
{
    static constexpr char const* api = "https://slack.com/api/chat.stopStream";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = StopStreamReply;

    std::string                 channel;            // An encoded ID that represents a channel, private group, or DM
    std::time_t                 ts;                 // The timestamp of the streaming message.
    OptString                   markdown_text;      // Accepts message text formatted in markdown. Limit this field to 12,000 characters.
    BlockKit::OptBlocks         blocks;             // A list of blocks that will be rendered at the bottom of the finalized message.
    OptMetadata                 metadata;           // JSON object with event_type and event_payload fields, presented as a URL-encoded string. Metadata you post to Slack is accessible to any app or user who is a member of that workspace.
};

}

// Response objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::AppendStreamReply, ok, channel, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::StartStreamReply, ok, channel, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::StopStreamReply, ok, channel, ts, message);

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::AppendStream, channel, ts, markdown_text);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::StartStream, channel, thread_ts, markdown_text, recipient_user_id, recipient_team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::StopStream, channel, ts, markdown_text, blocks, metadata);

#endif
