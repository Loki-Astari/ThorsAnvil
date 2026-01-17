#ifndef THORSANVIL_SLACK_EVENT_MESSAGE_H
#define THORSANVIL_SLACK_EVENT_MESSAGE_H


// Documentation: https://docs.slack.dev/reference/events/message.channels/

#include "ThorsSlackConfig.h"
#include "SlackBlockKit.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// event_callback: message
{
  "token": "<XXX>",
  "team_id": "T095XJHH589",
  "context_team_id": "T095XJHH589",
  "context_enterprise_id": null,
  "api_app_id": "A09RQFXSEKC",
  "event":
  {
    "type": "message",
    "user": "U095XJHJ1J5",
    "ts": "1763965955.056429",
    "client_msg_id": "ab06af53-2535-4052-a8e8-e739b2766d9c",
    "text": "Plop",
    "team": "T095XJHH589",
    "blocks": [
      {
        "type": "rich_text",
        "block_id": "Yy7Sx",
        "elements": [
          {
            "type": "rich_text_section",
            "elements": [
              {
                "type": "text",
                "text": "Plop"
              }
            ]
          }
        ]
      }
    ],
    "channel": "<Your Slack Channel>",
    "event_ts": "1763965955.056429",
    "channel_type": "channel"
  },
  "type": "event_callback",
  "event_id": "Ev09UN4L40SX",
  "event_time": 1763965955,
  "authorizations": [
    {
      "enterprise_id": null,
      "team_id": "T095XJHH589",
      "user_id": "U09S3D8R00Z",
      "is_bot": true,
      "is_enterprise_install": false
    }
  ],
  "is_ext_shared_channel": false,
  "event_context": "4-eyJldCI6Im1lc3NhZ2UiLCJ0aWQiOiJUMDk1WEpISDU4OSIsImFpZCI6IkEwOVJRRlhTRUtDIiwiY2lkIjoiQzA5UlUyVVJZTVMifQ"
}
#endif

using OptString = std::optional<std::string>;
using OptInt = std::optional<int>;
using OptBool = std::optional<bool>;
using VecString = std::vector<std::string>;
using OptVecString = std::optional<VecString>;
struct Message
{
    //std::string                 type;                           // doc
    std::string                 user;                           // doc      C
    std::string                 ts;                             // doc      C
    std::string                 client_msg_id;                  //          C
    std::string                 text;                           // doc      C
    std::string                 team;                           //          C
    BlockKit::OptBlocks         blocks;                         //          C
    OptString                   channel;                        // doc
    OptString                   event_ts;                       // doc
    OptString                   channel_type;                   // doc
    // --
    OptString                   thread_ts;
    OptInt                      reply_count;
    OptInt                      reply_users_count;
    OptString                   latest_reply;
    OptVecString                reply_users;
    OptBool                     is_locked;
    OptBool                     subscribed;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::Message, message);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Message, user, ts, client_msg_id, text, team, blocks, channel, event_ts, channel_type, thread_ts, reply_count, reply_users_count, latest_reply, reply_users, is_locked, subscribed);

#endif
