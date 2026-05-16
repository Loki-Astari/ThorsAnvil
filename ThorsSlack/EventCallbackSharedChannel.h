#ifndef THORSANVIL_SLACK_EVENT_SHARED_CHANNEL_H
#define THORSANVIL_SLACK_EVENT_SHARED_CHANNEL_H

// Documentation: https://docs.slack.dev/reference/events/shared_channel_invite_accepted

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/shared_channel_invite_accepted
  "event":
  {
    "type": "shared_channel_invite_accepted",
    "approval_required": false,
    "invite": { "id": "I028YDERZSQ", "date_created": 1626876000, "date_invalid": 1628085600, "inviting_team": {...}, "inviting_user": {...}, "recipient_email": "...", "recipient_user_id": "..." },
    "channel": { "id": "C12345678", "is_private": false, "is_im": false, "name": "test-slack-connect" },
    "teams_in_channel": [ {...} ],
    "accepting_user": {...},
    "event_ts": "1626877800.000000"
  }
// https://docs.slack.dev/reference/events/shared_channel_invite_approved
  "event":
  {
    "type": "shared_channel_invite_approved",
    "invite": {...},
    "channel": {...},
    "approving_team_id": "T87654321",
    "teams_in_channel": [ {...} ],
    "approving_user": {...},
    "event_ts": "1626881400.000000"
  }
// https://docs.slack.dev/reference/events/shared_channel_invite_declined
  "event":
  {
    "type": "shared_channel_invite_declined",
    "invite": {...},
    "channel": {...},
    "declining_team_id": "T87654321",
    "teams_in_channel": [ {...} ],
    "declining_user": {...},
    "event_ts": "1626881400.000000"
  }
// https://docs.slack.dev/reference/events/shared_channel_invite_received
  "event":
  {
    "type": "shared_channel_invite_received",
    "invite": {...},
    "channel": {...},
    "event_ts": "1626876010.000100"
  }
// https://docs.slack.dev/reference/events/shared_channel_invite_requested
  "event":
  {
    "type": "shared_channel_invite_requested",
    "channel": {...},
    "actor": {...},
    "is_external_limited": true,
    "target_users": [ { "invite_id": "I01234ABC", "email": "mock-user@some-corp.com" } ],
    "teams_in_channel": [ {...} ]
  }
#endif

using OptString = std::optional<std::string>;
using OptBool = std::optional<bool>;

struct SharedChannelUserProfile
{
    std::string                 real_name;
    std::string                 display_name;
    std::string                 real_name_normalized;
    std::string                 display_name_normalized;
    std::string                 team;
    std::string                 avatar_hash;
    std::string                 email;
    std::string                 image_24;
    std::string                 image_32;
    std::string                 image_48;
    std::string                 image_72;
    std::string                 image_192;
    std::string                 image_512;
};

struct SharedChannelUser
{
    std::string                 id;
    std::string                 team_id;
    std::string                 name;
    std::time_t                 updated;
    SharedChannelUserProfile    profile;
    OptString                   who_can_share_contact_card;
};

struct SharedChannelTeamIcon
{
    OptString                   image_34;
    OptBool                     image_default;
};

struct SharedChannelTeam
{
    std::string                 id;
    std::string                 name;
    SharedChannelTeamIcon       icon;
    bool                        is_verified;
    std::string                 domain;
    std::time_t                 date_created;
    OptString                   avatar_base_url;
    OptBool                     requires_sponsorship;
};
using VecSharedChannelTeam = std::vector<SharedChannelTeam>;

struct SharedChannelInvite
{
    std::string                 id;
    std::time_t                 date_created;
    std::time_t                 date_invalid;
    SharedChannelTeam           inviting_team;
    SharedChannelUser           inviting_user;
    OptString                   recipient_email;
    OptString                   recipient_user_id;
};

struct SharedChannelInfo
{
    std::string                 id;
    bool                        is_private;
    bool                        is_im;
    std::string                 name;
    OptString                   date_created;
    OptString                   message_latest_counted_timestamp;
};

struct SharedChannelTargetUser
{
    std::string                 invite_id;
    std::string                 email;
};
using VecSharedChannelTargetUser = std::vector<SharedChannelTargetUser>;

struct SharedChannelInviteAccepted
{
    // std::string                 type;
    bool                        approval_required;
    SharedChannelInvite         invite;
    SharedChannelInfo           channel;
    VecSharedChannelTeam        teams_in_channel;
    SharedChannelUser           accepting_user;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/SharedChannelInviteAccepted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SharedChannelInviteAccepted, shared_channel_invite_accepted);
    ThorsAnvil_TypeFieldName(type);
};

struct SharedChannelInviteApproved
{
    // std::string                 type;
    SharedChannelInvite         invite;
    SharedChannelInfo           channel;
    std::string                 approving_team_id;
    VecSharedChannelTeam        teams_in_channel;
    SharedChannelUser           approving_user;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/SharedChannelInviteApproved"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SharedChannelInviteApproved, shared_channel_invite_approved);
    ThorsAnvil_TypeFieldName(type);
};

struct SharedChannelInviteDeclined
{
    // std::string                 type;
    SharedChannelInvite         invite;
    SharedChannelInfo           channel;
    std::string                 declining_team_id;
    VecSharedChannelTeam        teams_in_channel;
    SharedChannelUser           declining_user;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/SharedChannelInviteDeclined"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SharedChannelInviteDeclined, shared_channel_invite_declined);
    ThorsAnvil_TypeFieldName(type);
};

struct SharedChannelInviteReceived
{
    // std::string                 type;
    SharedChannelInvite         invite;
    SharedChannelInfo           channel;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/SharedChannelInviteReceived"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SharedChannelInviteReceived, shared_channel_invite_received);
    ThorsAnvil_TypeFieldName(type);
};

struct SharedChannelInviteRequested
{
    // std::string                 type;
    SharedChannelInfo           channel;
    SharedChannelUser           actor;
    bool                        is_external_limited;
    VecSharedChannelTargetUser  target_users;
    VecSharedChannelTeam        teams_in_channel;
    static std::string const& typeName() {static std::string const name{"Event/SharedChannelInviteRequested"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SharedChannelInviteRequested, shared_channel_invite_requested);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelUserProfile, real_name, display_name, real_name_normalized, display_name_normalized, team, avatar_hash, email, image_24, image_32, image_48, image_72, image_192, image_512);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelUser, id, team_id, name, updated, profile, who_can_share_contact_card);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelTeamIcon, image_34, image_default);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelTeam, id, name, icon, is_verified, domain, date_created, avatar_base_url, requires_sponsorship);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInvite, id, date_created, date_invalid, inviting_team, inviting_user, recipient_email, recipient_user_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInfo, id, is_private, is_im, name, date_created, message_latest_counted_timestamp);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelTargetUser, invite_id, email);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInviteAccepted, approval_required, invite, channel, teams_in_channel, accepting_user, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInviteApproved, invite, channel, approving_team_id, teams_in_channel, approving_user, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInviteDeclined, invite, channel, declining_team_id, teams_in_channel, declining_user, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInviteReceived, invite, channel, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedChannelInviteRequested, channel, actor, is_external_limited, target_users, teams_in_channel);

#endif
