#ifndef THORSANVIL_SLACK_EVENT_APP_MENTIONED_H
#define THORSANVIL_SLACK_EVENT_APP_MENTIONED_H

// Documentation: https://docs.slack.dev/reference/events/app_mention

#include "ThorsSlackConfig.h"
#include "SlackBlockKit.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// App Mentioned
  "event":
  {
    "type": "app_mention",
    "user": "U095XJHJ1J5",
    "ts": "1767315542.938769",
    "client_msg_id": "3a386705-f543-4381-9a0a-a7b08d09b526",
    "text": "<@U0A6EA93KEE> Hi there",
    "team": "T095XJHH589",
    "thread_ts": "1767305275.956729",
    "parent_user_id": "U095XJHJ1J5",
    "blocks": [
      {
        "type": "rich_text",
        "block_id": "BmjI0",
        "elements": [
          {
            "type": "rich_text_section",
            "elements": [
              {
                "type": "user",
                "user_id": "U0A6EA93KEE"
              },
              {
                "type": "text",
                "text": " Hi there"
              }
            ]
          }
        ]
      }
    ],
    "channel": "C09RU2URYMS",
    "event_ts": "1767315542.938769"
  }
#endif

struct AppMentioned
{
    // std::string                 type;
    std::string         user;
    std::string         ts;
    std::string         client_msg_id;
    std::string         text;
    std::string         team;
    std::string         thread_ts;
    std::string         parent_user_id;
    BlockKit::Blocks    blocks;
    std::string         channel;
    std::string         event_ts;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AppMentioned, app_mention);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AppMentioned, user, ts, client_msg_id, text, team, thread_ts, parent_user_id, blocks, channel, event_ts);

#endif
