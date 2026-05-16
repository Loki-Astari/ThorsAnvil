#ifndef THORSANVIL_SLACK_EVENT_IM_H
#define THORSANVIL_SLACK_EVENT_IM_H

// Documentation: https://docs.slack.dev/reference/events/im_open

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/im_close
  "event": { "type": "im_close", "user": "U024BE7LH", "channel": "D024BE91L" }
// https://docs.slack.dev/reference/events/im_created
  "event": { "type": "im_created", "user": "U024BE7LH", "channel": { "id": "C0123456", "is_im": true, "user": "U0123456", "created": 1355517521, "is_org_shared": false } }
// https://docs.slack.dev/reference/events/im_history_changed
  "event": { "type": "im_history_changed", "latest": "1358877455.000010", "ts": "1361482916.000003", "event_ts": "1361482916.000004" }
// https://docs.slack.dev/reference/events/im_open
  "event": { "type": "im_open", "user": "U024BE7LH", "channel": "D024BE91L" }
#endif

using OptString = std::optional<std::string>;

struct ImChannelInfo
{
    std::string                 id;
    bool                        is_im;
    std::string                 user;
    std::time_t                 created;
    bool                        is_org_shared;
};

struct ImClose
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"Event/ImClose"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ImClose, im_close);
    ThorsAnvil_TypeFieldName(type);
};

struct ImCreated
{
    // std::string                 type;
    std::string                 user;
    ImChannelInfo               channel;
    static std::string const& typeName() {static std::string const name{"Event/ImCreated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ImCreated, im_created);
    ThorsAnvil_TypeFieldName(type);
};

struct ImHistoryChanged
{
    // std::string                 type;
    std::string                 latest;
    std::string                 ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/ImHistoryChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ImHistoryChanged, im_history_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct ImOpen
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"Event/ImOpen"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ImOpen, im_open);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ImChannelInfo, id, is_im, user, created, is_org_shared);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ImClose, user, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ImCreated, user, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ImHistoryChanged, latest, ts, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ImOpen, user, channel);

#endif
