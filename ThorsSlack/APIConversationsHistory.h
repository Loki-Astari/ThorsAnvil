#ifndef THORSANVIL_SLACK_API_CONVERSATION_HISTORY_H
#define THORSANVIL_SLACK_API_CONVERSATION_HISTORY_H

#include "ThorsSlackConfig.h"
#include "API.h"

namespace ThorsAnvil::Slack::API::Conversation
{

#if 0
{
  "ok": true,
  "latest": "1767461437.883259",
  "oldest": "1767461437.883259",
  "messages": [
    {
      "user": "U095XJHJ1J5",
      "type": "message",
      "ts": "1767461437.883259",
      "client_msg_id": "f0b1f4b7-bc21-4ac1-a829-338815fbd722",
      "text": "Another day",
      "team": "T095XJHH589",
      "thread_ts": "1767461437.883259",
      "reply_count": 2,
      "reply_users_count": 2,
      "latest_reply": "1767461445.353529",
      "reply_users": [
        "U095XJHJ1J5",
        "U0A6EA93KEE"
      ],
      "is_locked": false,
      "subscribed": false,
      "blocks": [
        {
          "type": "rich_text",
          "block_id": "em1dk",
          "elements": [
            {
              "type": "rich_text_section",
              "elements": [
                {
                  "type": "text",
                  "text": "Another day"
                }
              ]
            }
          ]
        }
      ]
    }
  ],
  "has_more": false,
  "is_limited": false,
  "pin_count": 0,
  "channel_actions_ts": null,
  "channel_actions_count": 0
}
#endif

struct HistoryAction{};

struct HistoryReply
{
    bool                ok          = false;
    std::string         latest;
    std::string         oldest;
    VecMessage          messages;
    bool                has_more;
    bool                is_limited;
    int                 pin_count;
    HistoryAction*      channel_actions_ts;
    int                 channel_actions_count;
    ThorsAnvil_VariantSerializer(ThorsAnvil::Slack::API::Conversation::HistoryReply);
};

// Action Objects
struct History
{
    static constexpr char const* api = "https://slack.com/api/conversations.history";
    static constexpr Method method = Method::GET;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = HistoryReply;

    std::string         channel;                // Conversation ID to fetch history for.
    OptString           cursor;                 // Paginate through collections of data by setting the cursor parameter to a next_cursor attribute returned by a previous request's response_metadata.
                                                // Default value fetches the first "page" of the collection. See pagination for more detail.
    OptBool             include_all_metadata;   // Return all metadata associated with this message.
    OptBool             inclusive;              // Include messages with oldest or latest timestamps in results. Ignored unless either timestamp is specified.
    OptString           latest;                 // Only messages before this Unix timestamp will be included in results.
                                                // Default is the current time.
    OptString           oldest;                 // Only messages after this Unix timestamp will be included in results.
                                                // Default: 0
    OptInt              limit;                  // The maximum number of items to return.
                                                // Fewer than the requested number of items may be returned, even if the end of the conversation history hasn't been reached.
                                                // Maximum of 999.
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Conversation::HistoryAction);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Conversation::HistoryReply, ok, latest, oldest, messages, has_more, is_limited, pin_count, channel_actions_ts, channel_actions_count);

// Action Objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Conversation::History, channel, cursor, include_all_metadata, inclusive, latest, oldest, limit);

#endif
