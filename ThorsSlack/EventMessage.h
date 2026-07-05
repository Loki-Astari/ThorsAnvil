#ifndef THORSANVIL_SLACK_EVENT_MESSAGE_H
#define THORSANVIL_SLACK_EVENT_MESSAGE_H


// Documentation: https://docs.slack.dev/reference/events/message.channels/

#include "ThorsSlackConfig.h"
#include "BlockKit.h"
#include "EventsFile.h"
#include <string>
#include <type_traits>

namespace ThorsAnvil::Slack::API
{

using VecString = std::vector<std::string>;

struct BotIcon
{
    std::string                 image_36;
    std::string                 image_48;
    std::string                 image_72;
};

struct BotProfile
{
    std::string                 id;
    std::string                 app_id;
    std::string                 user_id;
    std::string                 name;
    BotIcon                     icons;
    bool                        deleted;
    std::time_t                 updated;
    std::string                 team_id;
};
using OptBotProfile = std::optional<BotProfile>;

struct Reaction
{
    std::string                 name;
    VecString                   users;
    int                         count;
};
using VecReaction = std::vector<Reaction>;
using OptVecReaction = std::optional<VecReaction>;

}

namespace ThorsAnvil::Slack::Event
{

using OptString = std::optional<std::string>;
using OptNullString = std::optional<std::string>;
using OptInt = std::optional<int>;
using OptBool = std::optional<bool>;
using VecString = std::vector<std::string>;
using OptVecString = std::optional<VecString>;

// Based on:
//      https://docs.slack.dev/reference/events/message
//          https://docs.slack.dev/reference/events/message/assistant_app_thread/

//https://docs.slack.dev/reference/events/message/assistant_app_thread/
struct Edited
{
    std::string                 user;
    std::string                 ts;
};
using OptEdited = std::optional<Edited>;
//https://docs.slack.dev/reference/events/message/assistant_app_thread/
struct Artifact
{
};
//https://docs.slack.dev/reference/events/message/assistant_app_thread/
struct Assistant
{
    std::string                 title;
    BlockKit::Blocks            title_blocks;
    std::vector<Artifact>       artifacts;
};
using OptAssistant = std::optional<Assistant>;
// https://docs.slack.dev/reference/events/message/bot_message/
struct Icons
{
};
using OptIcons = std::optional<Icons>;
// https://docs.slack.dev/reference/events/message/document_mention
struct DocumentMention
{
    std::string                 file_id;
    std::string                 section_id;
    std::vector<std::string>    mentioning_user_ids;
};
using DocumentMentions = std::vector<DocumentMention>;
using OptDocumentMentions = std::optional<DocumentMentions>;
// https://docs.slack.dev/reference/events/message/file_comment
// https://docs.slack.dev/reference/events/message/file_mention
// https://docs.slack.dev/reference/events/message/file_share
struct File
{
    std::string                 id;
    std::time_t                 created;
    std::time_t                 timestamp;
    std::string                 name;
    std::string                 title;
    std::string                 mimetype;
    std::string                 filetype;
    std::string                 pretty_type;
    std::string                 user;
    bool                        editable;
    std::size_t                 size;
    std::string                 mode;
    bool                        is_external;
    std::string                 external_type;
    bool                        is_public;
    bool                        public_url_shared;
    bool                        display_as_bot;
    std::string                 username;
    std::string                 url_private;
    std::string                 url_private_download;
    std::string                 thumb_64;
    std::string                 thumb_80;
    std::string                 thumb_360;
    int                         thumb_360_w;
    int                         thumb_360_h;
    std::string                 thumb_480;
    int                         thumb_480_w;
    int                         thumb_480_h;
    std::string                 thumb_160;
    int                         image_exif_rotation;
    int                         original_w;
    int                         original_h;
    std::string                 pjpeg;
    std::string                 permalink;
    std::string                 permalink_public;
    bool                        has_rich_preview;
};
using OptFile = std::optional<File>;
using VecFile = std::vector<File>;
using OptVecFile = std::optional<VecFile>;
// https://docs.slack.dev/reference/events/message/message_replied
struct Reply
{
    std::string                 user;
    std::string                 ts;
};
using VecReply = std::vector<Reply>;
using OptVecReply = std::optional<VecReply>;

// https://docs.slack.dev/reference/events/message/pinned_item
struct Item
{
};
using OptItem = std::optional<Item>;


struct Message
{
    //std::string                 type;                               // message  (document_mention)

    // Common
        // https://docs.slack.dev/reference/events/message/assistant_app_thread/
        // https://docs.slack.dev/reference/events/message/bot_message/
        // https://docs.slack.dev/reference/events/message/channel_archive
        // https://docs.slack.dev/reference/events/message/channel_convert_to_private
        // https://docs.slack.dev/reference/events/message/channel_convert_to_public
        // https://docs.slack.dev/reference/events/message/channel_join
        // https://docs.slack.dev/reference/events/message/channel_leave
        // https://docs.slack.dev/reference/events/message/channel_name
        // https://docs.slack.dev/reference/events/message/channel_posting_permissions
        // https://docs.slack.dev/reference/events/message/channel_purpose
        // https://docs.slack.dev/reference/events/message/channel_topic
        // https://docs.slack.dev/reference/events/message/channel_unarchive
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/ekm_access_denied
        // https://docs.slack.dev/reference/events/message/file_comment
        // https://docs.slack.dev/reference/events/message/file_mention
        // https://docs.slack.dev/reference/events/message/file_share
        // https://docs.slack.dev/reference/events/message/group_archive
        // https://docs.slack.dev/reference/events/message/group_join
        // https://docs.slack.dev/reference/events/message/group_leave
        // https://docs.slack.dev/reference/events/message/group_name
        // https://docs.slack.dev/reference/events/message/group_purpose
        // https://docs.slack.dev/reference/events/message/group_topic
        // https://docs.slack.dev/reference/events/message/group_unarchive
        // https://docs.slack.dev/reference/events/message/me_message
        // https://docs.slack.dev/reference/events/message/message_changed
        // https://docs.slack.dev/reference/events/message/message_deleted
        // https://docs.slack.dev/reference/events/message/message_replied
        // https://docs.slack.dev/reference/events/message/pinned_item
        // https://docs.slack.dev/reference/events/message/reminder_add
        // https://docs.slack.dev/reference/events/message/thread_broadcast
        // https://docs.slack.dev/reference/events/message/unpinned_item
    std::string                 subtype;                            // assistant_app_thread / bot_message / channel_archive / channel_convert_to_private / channel_convert_to_public
                                                                    // channel_join / channel_name / channel_posting_permissions / channel_purpose / channel_topic / document_mention
                                                                    // ekm_access_denied / file_comment / file_mention / group_archive / group_join / group_leave / group_name
                                                                    // group_purpose / group_topic / group_unarchive / me_message / message_changed / message_deleted / message_replied
                                                                    // pinned_item / reminder_add / thread_broadcast / unpinned_item
                                          // subtype NOT: file_share
    std::string                 text;
                                          // text NOT: message_changed, message_deleted, message_replied
    std::string                 ts;

    // Common
        // https://docs.slack.dev/reference/events/message/assistant_app_thread/
        // https://docs.slack.dev/reference/events/message/channel_archive
        // https://docs.slack.dev/reference/events/message/channel_convert_to_private
        // https://docs.slack.dev/reference/events/message/channel_convert_to_public
        // https://docs.slack.dev/reference/events/message/channel_join
        // https://docs.slack.dev/reference/events/message/channel_leave
        // https://docs.slack.dev/reference/events/message/channel_name
        // https://docs.slack.dev/reference/events/message/channel_posting_permissions
        // https://docs.slack.dev/reference/events/message/channel_purpose
        // https://docs.slack.dev/reference/events/message/channel_topic
        // https://docs.slack.dev/reference/events/message/channel_unarchive
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/ekm_access_denied
        // https://docs.slack.dev/reference/events/message/file_mention
        // https://docs.slack.dev/reference/events/message/file_share
        // https://docs.slack.dev/reference/events/message/group_archive
        // https://docs.slack.dev/reference/events/message/group_join
        // https://docs.slack.dev/reference/events/message/group_leave
        // https://docs.slack.dev/reference/events/message/group_name
        // https://docs.slack.dev/reference/events/message/group_purpose
        // https://docs.slack.dev/reference/events/message/group_topic
        // https://docs.slack.dev/reference/events/message/group_unarchive
        // https://docs.slack.dev/reference/events/message/me_message
        // https://docs.slack.dev/reference/events/message/pinned_item
        // https://docs.slack.dev/reference/events/message/reminder_add
        // https://docs.slack.dev/reference/events/message/thread_broadcast
        // https://docs.slack.dev/reference/events/message/unpinned_item
    std::string                 user;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_convert_to_private
        // https://docs.slack.dev/reference/events/message/channel_convert_to_public
        // https://docs.slack.dev/reference/events/message/channel_posting_permissions
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/file_share
        // https://docs.slack.dev/reference/events/message/me_message
        // https://docs.slack.dev/reference/events/message/message_changed
        // https://docs.slack.dev/reference/events/message/message_deleted
        // https://docs.slack.dev/reference/events/message/message_replied
        // https://docs.slack.dev/reference/events/message/pinned_item
        // https://docs.slack.dev/reference/events/message/thread_broadcast
        // https://docs.slack.dev/reference/events/message/unpinned_item
    OptString                   channel;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_convert_to_private
        // https://docs.slack.dev/reference/events/message/channel_convert_to_public
        // https://docs.slack.dev/reference/events/message/channel_posting_permissions
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/file_share
        // https://docs.slack.dev/reference/events/message/message_replied
        // https://docs.slack.dev/reference/events/message/thread_broadcast
    OptString                   event_ts;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_convert_to_private
        // https://docs.slack.dev/reference/events/message/channel_convert_to_public
        // https://docs.slack.dev/reference/events/message/channel_posting_permissions
        // https://docs.slack.dev/reference/events/message/file_share
        // https://docs.slack.dev/reference/events/message/thread_broadcast
    OptString                   channel_type;

    // Common
        // https://docs.slack.dev/reference/events/message/assistant_app_thread/
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/reminder_add
    OptString                   team;

    // Common
        // https://docs.slack.dev/reference/events/message/assistant_app_thread/
        // https://docs.slack.dev/reference/events/message/document_mention
        // https://docs.slack.dev/reference/events/message/thread_broadcast
    BlockKit::Blocks            blocks;

    // Common
        // https://docs.slack.dev/reference/events/message/file_comment
        // https://docs.slack.dev/reference/events/message/file_mention
    OptFile                     file;

    // Common
        // https://docs.slack.dev/reference/events/message/bot_message/
        // https://docs.slack.dev/reference/events/message/file_share
    OptNullString               bot_id;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_name
        // https://docs.slack.dev/reference/events/message/group_name
    OptString                   old_name;
    OptString                   name;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_purpose
        // https://docs.slack.dev/reference/events/message/group_purpose
    OptString                   purpose;

    // Common
        // https://docs.slack.dev/reference/events/message/channel_topic
        // https://docs.slack.dev/reference/events/message/group_topic
    OptString                   topic;

    // Common
        // https://docs.slack.dev/reference/events/message/message_changed
        // https://docs.slack.dev/reference/events/message/message_deleted
        // https://docs.slack.dev/reference/events/message/message_replied
    OptBool                     hidden;


    // Common
        //https://docs.slack.dev/reference/events/message/assistant_app_thread/
        // https://docs.slack.dev/reference/events/message/thread_broadcast
    OptString                   thread_ts;

    // Common
        // https://docs.slack.dev/reference/events/message/pinned_item
        // https://docs.slack.dev/reference/events/message/unpinned_item
    OptString                   item_type;
    OptItem                     item;

    //https://docs.slack.dev/reference/events/message/assistant_app_thread/
    OptEdited                   edited;
    OptInt                      reply_count;
    OptInt                      reply_users_count;
    OptString                   latest_reply;
    OptVecString                reply_users;
    OptBool                     is_locked;
    OptAssistant                assistant_app_thread;

    // https://docs.slack.dev/reference/events/message/bot_message/
    OptString                   username;
    OptIcons                    icons;

    // https://docs.slack.dev/reference/events/message/channel_archive
    // No Unique.

    // https://docs.slack.dev/reference/events/message/channel_convert_to_private
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_convert_to_public
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_join
    OptString                   inviter;

    // https://docs.slack.dev/reference/events/message/channel_leave
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_name
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_posting_permissions
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_purpose
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_topic
    // No Unique

    // https://docs.slack.dev/reference/events/message/channel_unarchive
    // No Unique

    // https://docs.slack.dev/reference/events/message/document_mention
    OptDocumentMentions         document_mention;

    // https://docs.slack.dev/reference/events/message/ekm_access_denied
    // No Unique

    // https://docs.slack.dev/reference/events/message/file_comment
    OptFileComment                  comment;

    // https://docs.slack.dev/reference/events/message/file_mention
    // No Unique

    // https://docs.slack.dev/reference/events/message/file_share
    OptVecFile                  files;
    OptBool                     upload;
    OptBool                     display_as_bot;

    // https://docs.slack.dev/reference/events/message/group_archive
    OptVecString                members;

    // https://docs.slack.dev/reference/events/message/group_join
    // No Unique

    // https://docs.slack.dev/reference/events/message/group_leave
    // No Unique

    // https://docs.slack.dev/reference/events/message/group_name
    // No Unique

    // https://docs.slack.dev/reference/events/message/group_purpose
    // No Unique

    // https://docs.slack.dev/reference/events/message/group_topic
    // No Unique

    // https://docs.slack.dev/reference/events/message/group_unarchive
    // No Unique

    // https://docs.slack.dev/reference/events/message/me_message
    // No Unique

    // https://docs.slack.dev/reference/events/message/message_changed
    // std::optional<Message>   message;

    // https://docs.slack.dev/reference/events/message/message_deleted
    OptString                   deleted_ts;

    // https://docs.slack.dev/reference/events/message/message_replied
    // std::optional<Message>   message;
    OptVecReply                 replies;

    // https://docs.slack.dev/reference/events/message/pinned_item
    // No Unique

    // https://docs.slack.dev/reference/events/message/reminder_add
    // No Unique


    // https://docs.slack.dev/reference/events/message/thread_broadcast
    // std::optional<Message>   root;
    OptString                   client_msg_id;

    // https://docs.slack.dev/reference/events/message/unpinned_item
    // No Unique

    // Fields that may be returned by API requests.
    OptString                   app_id;
    OptBool                     subscribed;
    API::OptBotProfile          bot_profile;
    OptVecString                pinned_to;
    OptString                   permalink;
    API::OptVecReaction         reactions;

    // ThorsAnvil Type System.
    static std::string const& typeName() {static std::string const name{"Message"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::Message, message);
    ThorsAnvil_TypeFieldName(type);
};
using OptMessage = std::optional<Message>;
using VecMessage = std::vector<Message>;
using OptVecMessage = std::optional<VecMessage>;


}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::BotProfile,          id, app_id, user_id, name, icons, deleted, updated, team_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Reaction,            name, users, count);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::BotIcon,             image_36, image_48, image_72);

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Edited,            user, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Artifact);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Assistant,         title, title_blocks, artifacts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Icons);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::DocumentMention,   file_id, section_id, mentioning_user_ids);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::File,              id, created, timestamp, name, title, mimetype, filetype, pretty_type, user, editable, size, mode, is_external, external_type, is_public, public_url_shared, display_as_bot, username, url_private, url_private_download, thumb_64, thumb_80, thumb_360, thumb_360_w, thumb_360_h, thumb_480, thumb_480_w, thumb_480_h, thumb_160, image_exif_rotation, original_w, original_h, pjpeg, permalink, permalink_public, has_rich_preview);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Reply,             user, ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Item);
// ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Message,           subtype, ts, text, user, channel, event_ts, channel_type, team, blocks, file, bot_id, old_name, name, purpose, topic, hidden, thread_ts, item_type, item, edited, reply_count, reply_users_count, latest_reply, reply_users, is_locked, assistant_app_thread, username, icons, inviter, document_mention, comment, files, upload, display_as_bot, members, deleted_ts, replies, client_msg_id, app_id, subscribed, bot_profile, pinned_to, permalink, reactions);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Message,           subtype, ts, text, user, channel, event_ts, channel_type, team, blocks, file, bot_id, old_name, name, purpose, topic, hidden, thread_ts, item_type, item, edited, reply_count, reply_users_count, latest_reply, reply_users, is_locked, assistant_app_thread, username, icons, inviter, document_mention, comment, files, upload, display_as_bot, members, deleted_ts, replies, client_msg_id, app_id, subscribed, bot_profile, pinned_to, reactions, permalink);


#endif
