#ifndef THORSANVIL_SLACK_EVENT_STAR_ADDED_H
#define THORSANVIL_SLACK_EVENT_STAR_ADDED_H

// Documentation: https://docs.slack.dev/reference/events/reaction_added/

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/star_added
// Pin Added
  "event":
  {
    "type": "star_added",
    "user": "U095XJHJ1J5",
    "item":
    {
      "type": "channel",
      "channel": "<Your Slack Channel>",
      "date_create": 1764547799
    },
    "event_ts": "1764547799.000100"
  },
// https://docs.slack.dev/reference/events/star_removed
// Pin Removed
  "event":
  {
    "type": "star_removed",
    "user": "U095XJHJ1J5",
    "item":
    {
      "type": "channel",
      "channel": "<Your Slack Channel>",
      "date_create": 0
    },
    "event_ts": "1764547801.000200"
  },
#endif

struct StarItem
{
    std::string                 type;                   // doc
    std::string                 channel;                // doc
    std::time_t                 date_create;            // doc
};
struct StarAdded
{
    // std::string                 type;                   // doc
    std::string                 user;
    StarItem                    item;
    std::string                 event_ts;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::StarAdded, star_added);
    ThorsAnvil_TypeFieldName(type);
};
struct StarRemoved
{
    // std::string                 type;
    std::string                 user;
    StarItem                    item;
    std::string                 event_ts;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::StarRemoved, star_removed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::StarItem, type, channel, date_create);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::StarAdded, user, item, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::StarRemoved, user, item, event_ts);

#endif
