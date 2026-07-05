#ifndef THORSANVIL_SLACK_EVENT_FILE_H
#define THORSANVIL_SLACK_EVENT_FILE_H

// Documentation: https://docs.slack.dev/reference/events/file_created

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <ctime>
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/file_change
  "event": { "type": "file_change", "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_comment_added
  "event": { "type": "file_comment_added", "comment": { "id": "Fc67890", "comment": "text", "user": "U123ABC456", "created": 1360782804 }, "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_comment_edited
  "event": { "type": "file_comment_edited", "comment": { "id": "Fc67890", "comment": "text", "user": "U123ABC456", "created": 1360782804 }, "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_comment_deleted
  "event": { "type": "file_comment_deleted", "comment": "Fc67890", "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_created
  "event": { "type": "file_created", "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_deleted
  "event": { "type": "file_deleted", "file_id": "F2147483862", "event_ts": "1361482916.000004" }
// https://docs.slack.dev/reference/events/file_public
  "event": { "type": "file_public", "file_id": "F2147483862", "file": { "id": "F2147483862" } }
// https://docs.slack.dev/reference/events/file_shared
  "event": { "type": "file_shared", "channel_id": "D01315BHHSN", "file_id": "F2147483862", "user_id": "U0Z7K8SRH", "file": { "id": "F2147483862" }, "event_ts": "1617804931.000300" }
// https://docs.slack.dev/reference/events/file_unshared
  "event": { "type": "file_unshared", "file_id": "F2147483862", "file": { "id": "F2147483862" } }
#endif

using OptString = std::optional<std::string>;

struct FileRef
{
    std::string                 id;
};
using OptFileRef = std::optional<FileRef>;

struct FileComment
{
    std::string                 id;
    std::string                 comment;
    std::string                 user;
    std::time_t                 created;
};
using OptFileComment = std::optional<FileComment>;

struct FileChange
{
    // std::string                 type;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileChange"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileChange, file_change);
    ThorsAnvil_TypeFieldName(type);
};

struct FileCommentAdded
{
    // std::string                 type;
    FileComment                 comment;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileCommentAdded"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileCommentAdded, file_comment_added);
    ThorsAnvil_TypeFieldName(type);
};

struct FileCommentEdited
{
    // std::string                 type;
    FileComment                 comment;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileCommentEdited"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileCommentEdited, file_comment_edited);
    ThorsAnvil_TypeFieldName(type);
};

struct FileCommentDeleted
{
    // std::string                 type;
    std::string                 comment;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileCommentDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileCommentDeleted, file_comment_deleted);
    ThorsAnvil_TypeFieldName(type);
};

struct FileCreated
{
    // std::string                 type;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileCreated"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileCreated, file_created);
    ThorsAnvil_TypeFieldName(type);
};

struct FileDeleted
{
    // std::string                 type;
    std::string                 file_id;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"FileDeleted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileDeleted, file_deleted);
    ThorsAnvil_TypeFieldName(type);
};

struct FilePublic
{
    // std::string                 type;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FilePublic"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FilePublic, file_public);
    ThorsAnvil_TypeFieldName(type);
};

struct FileShared
{
    // std::string                 type;
    std::string                 channel_id;
    std::string                 file_id;
    std::string                 user_id;
    FileRef                     file;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"FileShared"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileShared, file_shared);
    ThorsAnvil_TypeFieldName(type);
};

struct FileUnshared
{
    // std::string                 type;
    std::string                 file_id;
    FileRef                     file;
    static std::string const& typeName() {static std::string const name{"FileUnshared"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::FileUnshared, file_unshared);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileRef, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileComment, id, comment, user, created);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileChange, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileCommentAdded, comment, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileCommentEdited, comment, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileCommentDeleted, comment, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileCreated, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileDeleted, file_id, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FilePublic, file_id, file);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileShared, channel_id, file_id, user_id, file, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::FileUnshared, file_id, file);

#endif
