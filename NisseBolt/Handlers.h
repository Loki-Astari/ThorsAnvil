#ifndef  THORSANVIL_NISSE_BOLT_HANDLERS_H
#define  THORSANVIL_NISSE_BOLT_HANDLERS_H

#include "NisseBoltConfig.h"
#include "Ack.h"
#include "Say.h"
#include "Response.h"

#include "ThorsSlack/SlackEventHandler.h"

#include <functional>
#include <variant>
#include <string>

namespace ThorsAnvil::Nisse::Bolt
{
namespace Event = ThorsAnvil::Slack::Event;

template<typename T>
using EventHandler          = std::function<void(T const& event, Say const& say)>;
using AnyEventHandler       = std::variant<
                                        EventHandler<Event::AppDeleted>, EventHandler<Event::AppHomeOpened>, EventHandler<Event::AppInstalled>, EventHandler<Event::AppRateLimited>, EventHandler<Event::AppRequested>, EventHandler<Event::AppUninstalledTeam>, EventHandler<Event::AppUninstalled>, EventHandler<Event::AppMentioned>,
                                        EventHandler<Event::AssistantThreadContextChanged>, EventHandler<Event::AssistantThreadStarted>,
                                        EventHandler<Event::CallRejected>,
                                        EventHandler<Event::ChannelArchive>, EventHandler<Event::ChannelCreated>, EventHandler<Event::ChannelDeleted>, EventHandler<Event::ChannelHistoryChanged>, EventHandler<Event::ChannelIdChanged>, EventHandler<Event::ChannelLeft>, EventHandler<Event::ChannelPostingPermissions>, EventHandler<Event::ChannelRename>, EventHandler<Event::ChannelShared>, EventHandler<Event::ChannelUnshared>,
                                        EventHandler<Event::DndUpdated>, EventHandler<Event::DndUpdatedUser>,
                                        EventHandler<Event::EmailDomainChanged>,
                                        EventHandler<Event::EmojiChanged>,
                                        EventHandler<Event::EntityDetailsRequested>,
                                        EventHandler<Event::FileChange>, EventHandler<Event::FileCommentAdded>, EventHandler<Event::FileCommentDeleted>, EventHandler<Event::FileCommentEdited>, EventHandler<Event::FileCreated>, EventHandler<Event::FileDeleted>, EventHandler<Event::FilePublic>, EventHandler<Event::FileShared>, EventHandler<Event::FileUnshared>,
                                        EventHandler<Event::FunctionExecuted>,
                                        EventHandler<Event::GridMigrationFinished>, EventHandler<Event::GridMigrationStarted>,
                                        EventHandler<Event::GroupClose>, EventHandler<Event::GroupDeleted>, EventHandler<Event::GroupHistoryChanged>, EventHandler<Event::GroupLeft>, EventHandler<Event::GroupOpen>, EventHandler<Event::GroupRename>,
                                        EventHandler<Event::ImClose>, EventHandler<Event::ImCreated>, EventHandler<Event::ImHistoryChanged>, EventHandler<Event::ImOpen>,
                                        EventHandler<Event::InviteRequested>,
                                        EventHandler<Event::LinkShared>,
                                        EventHandler<Event::MemberJoinedChannel>, EventHandler<Event::MemberLeftChannel>,
                                        // EventHandler<Event::Message>,
                                        EventHandler<Event::MessageMetadataPosted>, EventHandler<Event::MessageMetadataUpdated>, EventHandler<Event::MessageMetadataDeleted>,
                                        EventHandler<Event::PinAdded>, EventHandler<Event::PinRemoved>,
                                        EventHandler<Event::ReactionAdded>, EventHandler<Event::ReactionRemoved>,
                                        EventHandler<Event::SharedChannelInviteAccepted>, EventHandler<Event::SharedChannelInviteApproved>, EventHandler<Event::SharedChannelInviteDeclined>, EventHandler<Event::SharedChannelInviteReceived>, EventHandler<Event::SharedChannelInviteRequested>,
                                        EventHandler<Event::StarAdded>, EventHandler<Event::StarRemoved>,
                                        EventHandler<Event::SubteamCreated>, EventHandler<Event::SubteamMembersChanged>, EventHandler<Event::SubteamSelfAdded>, EventHandler<Event::SubteamSelfRemoved>, EventHandler<Event::SubteamUpdated>,
                                        EventHandler<Event::TeamAccessGranted>, EventHandler<Event::TeamAccessRevoked>, EventHandler<Event::TeamDomainChange>, EventHandler<Event::TeamJoin>, EventHandler<Event::TeamRename>,
                                        EventHandler<Event::TokensRevoked>,
                                        EventHandler<Event::UserChange>, EventHandler<Event::UserConnection>, EventHandler<Event::UserHuddleChanged>
                              >;

using Filter                = std::function<bool(ThorsAnvil::Slack::Event::Message const&)>;
using MessageHandler        = EventHandler<ThorsAnvil::Slack::Event::Message>;

using SlashCommandHandler   = std::function<void(SlashAck const&, Response const&, ThorsAnvil::Slack::SlashCommand const&)>;

using ActionHandler         = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::BlockActions const&, std::string const& value)>;

using ViewSubmitHandler     = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::Views::ViewSubmission const&)>;
using ViewClosedHandler     = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::API::Views::ViewClosed const&)>;

}

#endif
