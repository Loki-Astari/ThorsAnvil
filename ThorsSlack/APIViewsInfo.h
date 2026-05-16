#ifndef THORSANVIL_SLACK_API_VIEWS_REPLY_H
#define THORSANVIL_SLACK_API_VIEWS_REPLY_H


#include "ThorsSlackConfig.h"
#include "API.h"
#include "SlackBlockKit.h"
#include "APIBlockActionsState.h"
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::API::Views
{

using UPtrPlainText   = std::unique_ptr<BlockKit::PlainText>;
using UPtrString      = std::unique_ptr<std::string>;


struct ViewsInfo
{
    std::string                         id;
    std::string                         team_id;
    std::string                         type;
    BlockKit::PlainText                 title;
    BlockKit::Blocks                    blocks;
    UPtrPlainText                       close;
    UPtrPlainText                       submit;
    std::string                         private_metadata;
    std::string                         callback_id;
    bool                                clear_on_close;
    bool                                notify_on_close;
    std::string                         external_id;
    OptBool                             submit_disabled;
    UPtrString                          previous_view_id;
    SlackState                          state;
    std::string                         hash;
    std::string                         root_view_id;
    std::string                         app_id;
    std::string                         app_installed_team_id;
    std::string                         bot_id;
};

using OptViewsInfo = std::optional<ViewsInfo>;

}

// Response objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Views::ViewsInfo, id, team_id, type, title, blocks, close, submit, private_metadata, callback_id, clear_on_close, notify_on_close, external_id, submit_disabled, previous_view_id, state, hash, root_view_id, app_id, app_installed_team_id, bot_id);

#endif
