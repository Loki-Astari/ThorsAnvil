#ifndef THORSANVIL_SLACK_EVENT_MESSAGE_METADATA_H
#define THORSANVIL_SLACK_EVENT_MESSAGE_METADATA_H

// Documentation: https://docs.slack.dev/reference/events/message_metadata_posted

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <map>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/message_metadata_posted
  "event":
  {
    "type": "message_metadata_posted",
    "app_id": "AQF4F123M",
    "bot_id": "B8241P2B34D",
    "user_id": "U123ABC456",
    "team_id": "T12F3JCAP",
    "channel_id": "CJN879K8A",
    "metadata": { "event_type": "task_created", "event_payload": { ... } },
    "message_ts": "1658903885.673769",
    "event_ts": "1658903885.673769"
  }
// https://docs.slack.dev/reference/events/message_metadata_updated
  "event":
  {
    "type": "message_metadata_updated",
    "channel_id": "CJN879K8A",
    "event_ts": "1658906295.002200",
    "previous_metadata": { "event_type": "task_created", "event_payload": { ... } },
    "app_id": "AQF4F123M",
    "bot_id": "B8241P2B34D",
    "user_id": "U123ABC456",
    "team_id": "T12F3JCAP",
    "message_ts": "1658905974.587109",
    "metadata": { "event_type": "task_created", "event_payload": { ... } }
  }
// https://docs.slack.dev/reference/events/message_metadata_deleted
  "event":
  {
    "type": "message_metadata_deleted",
    "channel_id": "CJN879K8A",
    "event_ts": "1658907498.002500",
    "previous_metadata": { "event_type": "task_created", "event_payload": { ... } },
    "app_id": "AQF4F123M",
    "bot_id": "B8241P2B34D",
    "user_id": "U123ABC456",
    "team_id": "T12F3JCAP",
    "message_ts": "1658905974.587109",
    "deleted_ts": "1658907498.002500"
  }
#endif

using OptString = std::optional<std::string>;
using MetadataPayload = std::map<std::string, std::string>;

struct MessageMetadata
{
    std::string                 event_type;
    MetadataPayload             event_payload;
};
using OptMessageMetadata = std::optional<MessageMetadata>;

struct MessageMetadataPosted
{
    // std::string                 type;
    std::string                 app_id;
    std::string                 bot_id;
    std::string                 user_id;
    std::string                 team_id;
    std::string                 channel_id;
    MessageMetadata             metadata;
    std::string                 message_ts;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"MessageMetadataPosted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::MessageMetadataPosted, message_metadata_posted);
    ThorsAnvil_TypeFieldName(type);
};

struct MessageMetadataUpdated
{
    // std::string                 type;
    std::string                 channel_id;
    std::string                 event_ts;
    MessageMetadata             previous_metadata;
    std::string                 app_id;
    std::string                 bot_id;
    std::string                 user_id;
    std::string                 team_id;
    std::string                 message_ts;
    MessageMetadata             metadata;
    static std::string const& typeName() {static std::string const name{"MessageMetadataUpdated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::MessageMetadataUpdated, message_metadata_updated);
    ThorsAnvil_TypeFieldName(type);
};

struct MessageMetadataDeleted
{
    // std::string                 type;
    std::string                 channel_id;
    std::string                 event_ts;
    MessageMetadata             previous_metadata;
    std::string                 app_id;
    std::string                 bot_id;
    std::string                 user_id;
    std::string                 team_id;
    std::string                 message_ts;
    std::string                 deleted_ts;
    static std::string const& typeName() {static std::string const name{"MessageMetadataDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::MessageMetadataDeleted, message_metadata_deleted);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MessageMetadata, event_type, event_payload);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MessageMetadataPosted, app_id, bot_id, user_id, team_id, channel_id, metadata, message_ts, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MessageMetadataUpdated, channel_id, event_ts, previous_metadata, app_id, bot_id, user_id, team_id, message_ts, metadata);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::MessageMetadataDeleted, channel_id, event_ts, previous_metadata, app_id, bot_id, user_id, team_id, message_ts, deleted_ts);

#endif
