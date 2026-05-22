#ifndef THORSANVIL_SLACK_EVENT_EVENT_CALLBACK_H
#define THORSANVIL_SLACK_EVENT_EVENT_CALLBACK_H


// Documentation: https://docs.slack.dev/reference/events/message.channels/

#include "ThorsSlackConfig.h"

#include "EventsApp.h"
#include "EventAppMentioned.h"
#include "EventsAssistantThread.h"
#include "EventCallRejected.h"
#include "EventsChannel.h"
#include "EventsDnd.h"
#include "EventEmailDomainChanged.h"
#include "EventEmojiChanged.h"
#include "EventEntityDetailsRequested.h"
#include "EventsFile.h"
#include "EventFunctionExecuted.h"
#include "EventsGridMigration.h"
#include "EventsGroup.h"
#include "EventsIm.h"
#include "EventInviteRequested.h"
#include "EventLinkShared.h"
#include "EventsMember.h"
#include "EventMessage.h"
#include "EventsMessageMetadata.h"
#include "EventsPin.h"
#include "EventsReaction.h"
#include "EventsSharedChannel.h"
#include "EventsStar.h"
#include "EventsSubteam.h"
#include "EventsTeam.h"
#include "EventTokensRevoked.h"
#include "EventsUser.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"

#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace ThorsAnvil::Slack::Event
{

// using CallbackMessage = std::variant<Message, ReactionAdded, ReactionRemoved, PinAdded, PinRemoved, StarAdded, StarRemoved, AppMentioned>;
using CallbackMessage = std::variant<   AppDeleted, AppHomeOpened, AppInstalled, AppRateLimited, AppRequested, AppUninstalledTeam, AppUninstalled, AppMentioned,
                                        AssistantThreadContextChanged, AssistantThreadStarted,
                                        CallRejected,
                                        ChannelArchive, ChannelCreated, ChannelDeleted, ChannelHistoryChanged, ChannelIdChanged, ChannelLeft, ChannelPostingPermissions, ChannelRename, ChannelShared, ChannelUnshared,
                                        DndUpdated, DndUpdatedUser,
                                        EmailDomainChanged,
                                        EmojiChanged,
                                        EntityDetailsRequested,
                                        FileChange, FileCommentAdded, FileCommentDeleted, FileCommentEdited, FileCreated, FileDeleted, FilePublic, FileShared, FileUnshared,
                                        FunctionExecuted,
                                        GridMigrationFinished, GridMigrationStarted,
                                        GroupClose, GroupDeleted, GroupHistoryChanged, GroupLeft, GroupOpen, GroupRename,
                                        ImClose, ImCreated, ImHistoryChanged, ImOpen,
                                        InviteRequested,
                                        LinkShared,
                                        MemberJoinedChannel, MemberLeftChannel,
                                        Message,
                                        MessageMetadataPosted, MessageMetadataUpdated, MessageMetadataDeleted,
                                        PinAdded, PinRemoved,
                                        ReactionAdded, ReactionRemoved,
                                        SharedChannelInviteAccepted, SharedChannelInviteApproved, SharedChannelInviteDeclined, SharedChannelInviteReceived, SharedChannelInviteRequested,
                                        StarAdded, StarRemoved,
                                        SubteamCreated, SubteamMembersChanged, SubteamSelfAdded, SubteamSelfRemoved, SubteamUpdated,
                                        TeamAccessGranted, TeamAccessRevoked, TeamDomainChange, TeamJoin, TeamRename,
                                        TokensRevoked,
                                        UserChange, UserConnection, UserHuddleChanged
                                    >;

struct Enterprise
{};
struct Context
{};
struct Authorization
{
    Enterprise*                 enterprise_id   = nullptr;
    std::string                 team_id;
    std::string                 user_id;
    bool                        is_bot;
    bool                        is_enterprise_install;
};
using VecAuthorization = std::vector<Authorization>;
using Team = std::string;
using OptString = std::optional<std::string>;
using OptBool = std::optional<bool>;
using OptInt = std::optional<int>;
using VecTeam = std::vector<Team>;
using OptVecTeam = std::optional<VecTeam>;
using OptContext = std::optional<Context*>;

struct EventCallback
{
    std::string                 token;                          // doc      X
    std::string                 team_id;                        // doc      X
    OptString                   context_team_id;                //          X
    OptContext                  context_enterprise_id = nullptr;//          X
    std::string                 api_app_id;                     // doc      X
    CallbackMessage             event;                          // doc
    //std::string                 type;                           // doc    X
    std::string                 event_id;                       // doc      X
    std::size_t                 event_time;                     // doc      X
    VecAuthorization            authorizations;                 //          X
    bool                        is_ext_shared_channel;          //          X
    OptString                   event_context;                  //          X
    OptVecTeam                  authed_teams;                   // doc
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::EventCallback, event_callback);
    ThorsAnvil_TypeFieldName(type);
};


}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Enterprise);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Context);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::Authorization, enterprise_id, team_id, user_id, is_bot, is_enterprise_install);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EventCallback, token, team_id, context_team_id, context_enterprise_id, api_app_id, event, event_id, event_time, authorizations, is_ext_shared_channel, event_context, authed_teams);

#endif
