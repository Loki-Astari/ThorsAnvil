#ifndef THORSANVIL_SLACK_EVENT_EMOJI_CHANGED_H
#define THORSANVIL_SLACK_EVENT_EMOJI_CHANGED_H

// Documentation: https://docs.slack.dev/reference/events/emoji_changed

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/emoji_changed
// subtype: add
  "event":
  {
    "type": "emoji_changed",
    "subtype": "add",
    "name": "picard_facepalm",
    "value": "https://my.slack.com/emoji/picard_facepalm/db8e287430eaa459.gif",
    "event_ts": "1361482916.000004"
  }
// subtype: remove
  "event":
  {
    "type": "emoji_changed",
    "subtype": "remove",
    "names": ["picard_facepalm", "other"],
    "event_ts": "1361482916.000004"
  }
// subtype: rename
  "event":
  {
    "type": "emoji_changed",
    "subtype": "rename",
    "old_name": "picard_facepalm",
    "new_name": "picard_facepalm2",
    "value": "https://my.slack.com/emoji/picard_facepalm/db8e287430eaa459.gif",
    "event_ts": "1361482916.000004"
  }
#endif

using OptString = std::optional<std::string>;
using VecString = std::vector<std::string>;
using OptVecString = std::optional<VecString>;

struct EmojiChanged
{
    // std::string                 type;
    std::string                 subtype;
    OptString                   name;
    OptString                   value;
    OptVecString                names;
    OptString                   old_name;
    OptString                   new_name;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"EmojiChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::EmojiChanged, emoji_changed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EmojiChanged, subtype, name, value, names, old_name, new_name, event_ts);

#endif
