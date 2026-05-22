#ifndef THORSANVIL_SLACK_EVENT_INVITE_REQUESTED_H
#define THORSANVIL_SLACK_EVENT_INVITE_REQUESTED_H

// Documentation: https://docs.slack.dev/reference/events/invite_requested

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/invite_requested
  "event":
  {
    "type": "invite_requested",
    "invite_request":
    {
        "id": "12345",
        "email": "bront@puppies.com",
        "date_created": 123455,
        "requester_ids": ["U123ABC456"],
        "channel_ids": ["C123ABC456"],
        "invite_type": "full_member",
        "real_name": "Brent",
        "date_expire": 123456,
        "request_reason": "They're good dogs, Brant",
        "team": { "id": "T12345", "name": "Puppy ratings workspace incorporated", "domain": "puppiesrus" }
    }
  }
#endif

using VecString = std::vector<std::string>;
using OptString = std::optional<std::string>;

struct InviteRequestTeam
{
    std::string                 id;
    std::string                 name;
    std::string                 domain;
};

struct InviteRequest
{
    std::string                 id;
    std::string                 email;
    std::time_t                 date_created;
    VecString                   requester_ids;
    VecString                   channel_ids;
    std::string                 invite_type;
    std::string                 real_name;
    std::time_t                 date_expire;
    std::string                 request_reason;
    InviteRequestTeam           team;
};

struct InviteRequested
{
    // std::string                 type;
    InviteRequest               invite_request;
    static std::string const& typeName() {static std::string const name{"InviteRequested"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::InviteRequested, invite_requested);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::InviteRequestTeam, id, name, domain);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::InviteRequest, id, email, date_created, requester_ids, channel_ids, invite_type, real_name, date_expire, request_reason, team);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::InviteRequested, invite_request);

#endif
