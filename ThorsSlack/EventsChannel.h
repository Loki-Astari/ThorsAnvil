#ifndef THORSANVIL_SLACK_EVENT_CHANNEL_H
#define THORSANVIL_SLACK_EVENT_CHANNEL_H

// Documentation: https://docs.slack.dev/reference/events/channel_created

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/channel_archive
  "event": { "type": "channel_archive", "channel": "C123ABC456", "user": "U123ABC456" }
// https://docs.slack.dev/reference/events/channel_created
  "event": { "type": "channel_created", "channel": { "id": "C024BE91L", "name": "fun", "created": 1360782804, "creator": "U024BE7LH" } }
// https://docs.slack.dev/reference/events/channel_deleted
  "event": { "type": "channel_deleted", "channel": "C123ABC456" }
// https://docs.slack.dev/reference/events/channel_history_changed
  "event": { "type": "channel_history_changed", "latest": "1358877455.000010", "ts": "1361482916.000003", "event_ts": "1361482916.000004" }
// https://docs.slack.dev/reference/events/channel_id_changed
  "event": { "type": "channel_id_changed", "old_channel_id": "G012Y48650T", "new_channel_id": "C012Y48650T", "event_ts": "1612206778.000000" }
// https://docs.slack.dev/reference/events/channel_left
  "event": { "type": "channel_left", "channel": "C123ABC456" }
// https://docs.slack.dev/reference/events/channel_rename
  "event": { "type": "channel_rename", "channel": { "id": "C02ELGNBH", "name": "new_name", "created": 1360782804 } }
// https://docs.slack.dev/reference/events/channel_shared
  "event": { "type": "channel_shared", "connected_team_id": "E163Q94DX", "channel": "C123ABC456", "event_ts": "1561064063.001100" }
// https://docs.slack.dev/reference/events/channel_unshared
  "event": { "type": "channel_unshared", "previously_connected_team_id": "E163Q94DX", "channel": "C123ABC456", "is_ext_shared": false, "event_ts": "1561064063.001100" }
// https://docs.slack.dev/reference/events/channel_posting_permissions
  "event": { "type": "channel_posting_permissions", "user_id": "U123ABC456", "channel_id": "C123ABC456", "event_ts": "1234567890.123456" }
#endif

using OptString = std::optional<std::string>;

struct ChannelInfo
{
    std::string                 id;
    std::string                 name;
    std::time_t                 created;
    OptString                   creator;
};

struct ChannelArchive
{
    // std::string                 type;
    std::string                 channel;
    std::string                 user;
    static std::string const& typeName() {static std::string const name{"ChannelArchive"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelArchive, channel_archive);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelCreated
{
    // std::string                 type;
    ChannelInfo                 channel;
    static std::string const& typeName() {static std::string const name{"ChannelCreated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelCreated, channel_created);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelDeleted
{
    // std::string                 type;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"ChannelDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelDeleted, channel_deleted);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelHistoryChanged
{
    // std::string                 type;
    std::string                 latest;
    std::string                 ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"ChannelHistoryChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelHistoryChanged, channel_history_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelIdChanged
{
    // std::string                 type;
    std::string                 old_channel_id;
    std::string                 new_channel_id;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"ChannelIdChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelIdChanged, channel_id_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelLeft
{
    // std::string                 type;
    std::string                 channel;
    static std::string const& typeName() {static std::string const name{"ChannelLeft"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelLeft, channel_left);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelRename
{
    // std::string                 type;
    ChannelInfo                 channel;
    static std::string const& typeName() {static std::string const name{"ChannelRename"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelRename, channel_rename);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelShared
{
    // std::string                 type;
    std::string                 connected_team_id;
    std::string                 channel;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"ChannelShared"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelShared, channel_shared);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelUnshared
{
    // std::string                 type;
    std::string                 previously_connected_team_id;
    std::string                 channel;
    bool                        is_ext_shared;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"ChannelUnshared"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelUnshared, channel_unshared);
    ThorsAnvil_TypeFieldName(type);
};

struct ChannelPostingPermissions
{
    // std::string                 type;
    std::string                 user_id;
    std::string                 channel_id;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"ChannelPostingPermissions"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::ChannelPostingPermissions, channel_posting_permissions);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelInfo, id, name, created, creator);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelArchive, channel, user);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelCreated, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelDeleted, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelHistoryChanged, latest, ts, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelIdChanged, old_channel_id, new_channel_id, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelLeft, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelRename, channel);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelShared, connected_team_id, channel, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelUnshared, previously_connected_team_id, channel, is_ext_shared, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ChannelPostingPermissions, user_id, channel_id, event_ts);

#endif
