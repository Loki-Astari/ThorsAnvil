#ifndef THORSANVIL_SLACK_EVENT_GROUP_H
#define THORSANVIL_SLACK_EVENT_GROUP_H

// Documentation: https://docs.slack.dev/reference/events/group_archive

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/group_close
  "event": { "type": "group_close", "user": "U024BE7LH", "channel": "G024BE91L" }
// https://docs.slack.dev/reference/events/group_deleted
  "event": { "type": "group_deleted", "channel": "G0QN9RGTT" }
// https://docs.slack.dev/reference/events/group_history_changed
  "event": { "type": "group_history_changed", "latest": "1358877455.000010", "ts": "1361482916.000003", "event_ts": "1361482916.000004" }
// https://docs.slack.dev/reference/events/group_left
  "event": { "type": "group_left", "channel": "G02ELGNBH" }
// https://docs.slack.dev/reference/events/group_open
  "event": { "type": "group_open", "user": "U024BE7LH", "channel": "G024BE91L" }
// https://docs.slack.dev/reference/events/group_rename
  "event": { "type": "group_rename", "channel": { "id": "G02ELGNBH", "name": "new_name", "created": 1360782804 } }
#endif

using OptString = std::optional<std::string>;
using VecString = std::vector<std::string>;

struct GroupTopicPurpose
{
    std::string                 value;
    std::string                 creator;
    std::time_t                 last_set;
};
using OptGroupTopicPurpose = std::optional<GroupTopicPurpose>;

struct GroupChannelInfo
{
    std::string                 id;
    std::string                 name;
    OptString                   is_group;
    std::time_t                 created;
    OptString                   creator;
    OptString                   is_archived;
    OptString                   is_mpim;
    std::optional<VecString>    members;
    OptGroupTopicPurpose        topic;
    OptGroupTopicPurpose        purpose;
};

struct GroupRenameInfo
{
    std::string                 id;
    std::string                 name;
    std::time_t                 created;
};

struct GroupClose
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"GroupClose"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupClose, group_close);
    ThorsAnvil_TypeFieldName(type);
};

struct GroupDeleted
{
    // std::string                 type;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"GroupDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupDeleted, group_deleted);
    ThorsAnvil_TypeFieldName(type);
};

struct GroupHistoryChanged
{
    // std::string                 type;
    std::string                 latest;
    std::string                 ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"GroupHistoryChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupHistoryChanged, group_history_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct GroupLeft
{
    // std::string                 type;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"GroupLeft"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupLeft, group_left);
    ThorsAnvil_TypeFieldName(type);
};

struct GroupOpen
{
    // std::string                 type;
    std::string                 user;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"GroupOpen"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupOpen, group_open);
    ThorsAnvil_TypeFieldName(type);
};

struct GroupRename
{
    // std::string                 type;
    GroupRenameInfo              channel;
    static std::string const& typeName() {static std::string const name{"GroupRename"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::GroupRename, group_rename);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupTopicPurpose, value, creator, last_set);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupChannelInfo, id, name, is_group, created, creator, is_archived, is_mpim, members, topic, purpose);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupRenameInfo, id, name, created);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupClose, user, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupDeleted, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupHistoryChanged, latest, ts, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupLeft, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupOpen, user, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::GroupRename, channel);

#endif
