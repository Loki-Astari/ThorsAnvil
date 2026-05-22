#ifndef THORSANVIL_SLACK_EVENT_DND_H
#define THORSANVIL_SLACK_EVENT_DND_H

// Documentation: https://docs.slack.dev/reference/events/dnd_updated

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/dnd_updated
  "event":
  {
    "type": "dnd_updated",
    "user": "U1234",
    "dnd_status":
    {
        "dnd_enabled": true,
        "next_dnd_start_ts": 1450387800,
        "next_dnd_end_ts": 1450423800,
        "snooze_enabled": true,
        "snooze_endtime": 1450373897
    }
  }
// https://docs.slack.dev/reference/events/dnd_updated_user
  "event":
  {
    "type": "dnd_updated_user",
    "user": "U123ABC456",
    "dnd_status":
    {
        "dnd_enabled": true,
        "next_dnd_start_ts": 1450387800,
        "next_dnd_end_ts": 1450423800
    }
  }
#endif

using OptBool = std::optional<bool>;
using OptTime = std::optional<std::time_t>;

struct DndStatus
{
    bool                        dnd_enabled;
    std::time_t                 next_dnd_start_ts;
    std::time_t                 next_dnd_end_ts;
    OptBool                     snooze_enabled;
    OptTime                     snooze_endtime;
};

struct DndUpdated
{
    // std::string                 type;
    std::string                 user;
    DndStatus                   dnd_status;
    static std::string const& typeName() {static std::string const name{"DndUpdated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::DndUpdated, dnd_updated);
    ThorsAnvil_TypeFieldName(type);
};

struct DndUpdatedUser
{
    // std::string                 type;
    std::string                 user;
    DndStatus                   dnd_status;
    static std::string const& typeName() {static std::string const name{"DndUpdatedUser"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::DndUpdatedUser, dnd_updated_user);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::DndStatus, dnd_enabled, next_dnd_start_ts, next_dnd_end_ts, snooze_enabled, snooze_endtime);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::DndUpdated, user, dnd_status);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::DndUpdatedUser, user, dnd_status);

#endif
