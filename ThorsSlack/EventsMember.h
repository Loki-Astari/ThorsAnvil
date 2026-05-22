#ifndef THORSANVIL_SLACK_EVENT_MEMBER_H
#define THORSANVIL_SLACK_EVENT_MEMBER_H

// Documentation: https://docs.slack.dev/reference/events/member_joined_channel

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/member_joined_channel
  "event":
  {
    "type": "member_joined_channel",
    "user": "W123ABC456",
    "channel": "C123ABC456",
    "channel_type": "C",
    "team": "T123ABC456",
    "inviter": "U123456789",
    "enterprise": "E123456789"
  }
// https://docs.slack.dev/reference/events/member_left_channel
  "event":
  {
    "type": "member_left_channel",
    "user": "W123ABC456",
    "channel": "C123ABC456",
    "channel_type": "C",
    "team": "T123ABC456"
  }
#endif

using OptString = std::optional<std::string>;

struct MemberJoinedChannel
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    std::string                 channel_type;
    std::string                 team;
    OptString                   inviter;
    OptString                   enterprise;
    static std::string const& typeName() {static std::string const name{"MemberJoinedChannel"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::MemberJoinedChannel, member_joined_channel);
    ThorsAnvil_TypeFieldName(type);
};

struct MemberLeftChannel
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    std::string                 channel_type;
    std::string                 team;
    static std::string const& typeName() {static std::string const name{"MemberLeftChannel"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::MemberLeftChannel, member_left_channel);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MemberJoinedChannel, user, channel, channel_type, team, inviter, enterprise);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MemberLeftChannel, user, channel, channel_type, team);

#endif
