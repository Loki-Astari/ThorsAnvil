#ifndef THORSANVIL_SLACK_EVENT_TEAM_H
#define THORSANVIL_SLACK_EVENT_TEAM_H

// Documentation: https://docs.slack.dev/reference/events/team_join

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/team_access_granted
  "event": { "type": "team_access_granted", "team_ids": ["T1XX3", "TXX34"] }
// https://docs.slack.dev/reference/events/team_access_revoked
  "event": { "type": "team_access_revoked", "team_ids": ["T1XX3", "TXX34"] }
// https://docs.slack.dev/reference/events/team_domain_change
  "event": { "type": "team_domain_change", "url": "https://my.slack.com", "domain": "my", "team_id": "T1234" }
// https://docs.slack.dev/reference/events/team_join
  "event": { "type": "team_join", "user": {} }
// https://docs.slack.dev/reference/events/team_rename
  "event": { "type": "team_rename", "name": "New Team Name Inc.", "team_id": "T1234" }
#endif

using OptString = std::optional<std::string>;
using OptInt = std::optional<int>;
using VecString = std::vector<std::string>;

struct TeamJoinUser
{
    OptString                   id;
    OptString                   team_id;
    OptString                   name;
};

struct TeamProfileField
{
    std::string                 id;
    OptInt                      ordering;
};
using VecTeamProfileField = std::vector<TeamProfileField>;

struct TeamProfileChange
{
    VecTeamProfileField         fields;
};

struct TeamProfileDeleteFields
{
    VecString                   fields;
};

struct TeamAccessGranted
{
    // std::string                 type;
    VecString                   team_ids;
    static std::string const& typeName() {static std::string const name{"TeamAccessGranted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TeamAccessGranted, team_access_granted);
    ThorsAnvil_TypeFieldName(type);
};

struct TeamAccessRevoked
{
    // std::string                 type;
    VecString                   team_ids;
    static std::string const& typeName() {static std::string const name{"TeamAccessRevoked"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TeamAccessRevoked, team_access_revoked);
    ThorsAnvil_TypeFieldName(type);
};

struct TeamDomainChange
{
    // std::string                 type;
    std::string                 url;
    std::string                 domain;
    std::string                 team_id;
    static std::string const& typeName() {static std::string const name{"TeamDomainChange"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TeamDomainChange, team_domain_change);
    ThorsAnvil_TypeFieldName(type);
};

struct TeamJoin
{
    // std::string                 type;
    TeamJoinUser                user;
    static std::string const& typeName() {static std::string const name{"TeamJoin"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TeamJoin, team_join);
    ThorsAnvil_TypeFieldName(type);
};

struct TeamRename
{
    // std::string                 type;
    std::string                 name;
    std::string                 team_id;
    static std::string const& typeName() {static std::string const name{"TeamRename"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TeamRename, team_rename);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamJoinUser, id, team_id, name);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamProfileField, id, ordering);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamProfileChange, fields);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamProfileDeleteFields, fields);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamAccessGranted, team_ids);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamAccessRevoked, team_ids);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamDomainChange, url, domain, team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamJoin, user);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TeamRename, name, team_id);

#endif
