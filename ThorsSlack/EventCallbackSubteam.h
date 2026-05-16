#ifndef THORSANVIL_SLACK_EVENT_SUBTEAM_H
#define THORSANVIL_SLACK_EVENT_SUBTEAM_H

// Documentation: https://docs.slack.dev/reference/events/subteam_created

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/subteam_created
  "event":
  {
    "type": "subteam_created",
    "subteam":
    {
        "id": "S0615G0KT", "team_id": "T060RNRCH", "is_usergroup": true,
        "name": "Marketing Team", "description": "...", "handle": "marketing-team",
        "is_external": false, "date_create": 1446746793, "date_update": 1446746793, "date_delete": 0,
        "auto_type": null, "created_by": "U060RNRCZ", "updated_by": "U060RNRCZ", "deleted_by": null,
        "prefs": { "channels": [], "groups": [] }, "user_count": "0"
    }
  }
// https://docs.slack.dev/reference/events/subteam_members_changed
  "event":
  {
    "type": "subteam_members_changed",
    "subteam_id": "S123ABC456", "team_id": "T060RNRCH",
    "date_previous_update": 1446670362, "date_update": 1492906952,
    "added_users": ["U060RNRCZ", "U060ULRC0", "U061309JM"], "added_users_count": "3",
    "removed_users": ["U06129G2V"], "removed_users_count": "1"
  }
// https://docs.slack.dev/reference/events/subteam_self_added
  "event": { "type": "subteam_self_added", "subteam_id": "S0615G0KT" }
// https://docs.slack.dev/reference/events/subteam_self_removed
  "event": { "type": "subteam_self_removed", "subteam_id": "S0615G0KT" }
// https://docs.slack.dev/reference/events/subteam_updated
  "event":
  {
    "type": "subteam_updated",
    "subteam": { ... }
  }
#endif

using OptString = std::optional<std::string>;
using OptInt = std::optional<int>;
using OptBool = std::optional<bool>;
using VecString = std::vector<std::string>;
using OptVecString = std::optional<VecString>;

struct SubteamPrefs
{
    VecString                   channels;
    VecString                   groups;
};

struct SubteamInfo
{
    std::string                 id;
    std::string                 team_id;
    bool                        is_usergroup;
    std::string                 name;
    std::string                 description;
    std::string                 handle;
    bool                        is_external;
    std::time_t                 date_create;
    std::time_t                 date_update;
    std::time_t                 date_delete;
    OptString                   auto_type;
    OptBool                     auto_provision;
    OptString                   enterprise_subteam_id;
    std::string                 created_by;
    std::string                 updated_by;
    OptString                   deleted_by;
    SubteamPrefs                prefs;
    OptBool                     is_subteam;
    OptVecString                users;
    OptString                   user_count;
    OptInt                      channel_count;
};

struct SubteamCreated
{
    // std::string                 type;
    SubteamInfo                 subteam;
    static std::string const& typeName() {static std::string const name{"Event/SubteamCreated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SubteamCreated, subteam_created);
    ThorsAnvil_TypeFieldName(type);
};

struct SubteamMembersChanged
{
    // std::string                 type;
    std::string                 subteam_id;
    std::string                 team_id;
    std::time_t                 date_previous_update;
    std::time_t                 date_update;
    VecString                   added_users;
    std::string                 added_users_count;
    VecString                   removed_users;
    std::string                 removed_users_count;
    static std::string const& typeName() {static std::string const name{"Event/SubteamMembersChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SubteamMembersChanged, subteam_members_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct SubteamSelfAdded
{
    // std::string                 type;
    std::string                 subteam_id;
    static std::string const& typeName() {static std::string const name{"Event/SubteamSelfAdded"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SubteamSelfAdded, subteam_self_added);
    ThorsAnvil_TypeFieldName(type);
};

struct SubteamSelfRemoved
{
    // std::string                 type;
    std::string                 subteam_id;
    static std::string const& typeName() {static std::string const name{"Event/SubteamSelfRemoved"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SubteamSelfRemoved, subteam_self_removed);
    ThorsAnvil_TypeFieldName(type);
};

struct SubteamUpdated
{
    // std::string                 type;
    SubteamInfo                 subteam;
    static std::string const& typeName() {static std::string const name{"Event/SubteamUpdated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::SubteamUpdated, subteam_updated);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamPrefs, channels, groups);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamInfo, id, team_id, is_usergroup, name, description, handle, is_external, date_create, date_update, date_delete, auto_type, auto_provision, enterprise_subteam_id, created_by, updated_by, deleted_by, prefs, is_subteam, users, user_count, channel_count);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamCreated, subteam);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamMembersChanged, subteam_id, team_id, date_previous_update, date_update, added_users, added_users_count, removed_users, removed_users_count);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamSelfAdded, subteam_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamSelfRemoved, subteam_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SubteamUpdated, subteam);

#endif
