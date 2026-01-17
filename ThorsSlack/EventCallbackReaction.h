#ifndef THORSANVIL_SLACK_EVENT_REACTION_ADDED_H
#define THORSANVIL_SLACK_EVENT_REACTION_ADDED_H

// Documentation: https://docs.slack.dev/reference/events/reaction_added/

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/reaction_added
// Reaction Added
  "event":
  {
    "type": "reaction_added",
    "user": "U095XJHJ1J5",
    "reaction": "smiley",
    "item":
    {
      "type": "message",
      "channel": "<Your Slack Channel>",
      "ts": "1764544528.464179"
    },
    "item_user": "U09S3D8R00Z",
    "event_ts": "1764545087.005100"
  },
// https://docs.slack.dev/reference/events/reaction_removed
// Reaction Removed
  "event":
  {
    "type": "reaction_removed",
    "user": "U095XJHJ1J5",
    "reaction": "smiley",
    "item":
    {
      "type": "message",
      "channel": "<Your Slack Channel>",
      "ts": "1764544528.464179"
    },
    "item_user": "U09S3D8R00Z",
    "event_ts": "1764545209.005200"
  },
#endif

struct ReactionItem
{
    std::string                 type;                   // doc
    std::string                 channel;                // doc
    std::string                 ts;                     // doc
};
struct ReactionAdded
{
    // std::string                 type;                   // doc
    std::string                 user;                   // doc
    std::string                 reaction;               // doc
    ReactionItem                item;                   // doc
    std::string                 item_user;
    std::string                 event_ts;               // doc
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ReactionAdded, reaction_added);
    ThorsAnvil_TypeFieldName(type);
};
struct ReactionRemoved
{
    // std::string                 type;                   // doc
    std::string                 user;                   // doc
    std::string                 reaction;               // doc
    ReactionItem                item;                   // doc
    std::string                 item_user;
    std::string                 event_ts;               // doc
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ReactionRemoved, reaction_removed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ReactionItem, type, channel, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ReactionAdded, user, reaction, item, item_user, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ReactionRemoved, user, reaction, item, item_user, event_ts);

#endif
