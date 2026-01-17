#ifndef THORSANVIL_SLACK_EVENT_EVENT_CALLBACK_H
#define THORSANVIL_SLACK_EVENT_EVENT_CALLBACK_H


// Documentation: https://docs.slack.dev/reference/events/message.channels/

#include "ThorsSlackConfig.h"
#include "EventCallbackMessage.h"
#include "EventCallbackReaction.h"
#include "EventCallbackPin.h"
#include "EventCallbackStar.h"
#include "EventCallbackAppMentioned.h"
#include "ThorSerialize/Traits.h"
#include "ThorSerialize/SerUtil.h"
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace ThorsAnvil::Slack::Event
{

using CallbackMessage = std::variant<Message, ReactionAdded, ReactionRemoved, PinAdded, PinRemoved, StarAdded, StarRemoved, AppMentioned>;

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
