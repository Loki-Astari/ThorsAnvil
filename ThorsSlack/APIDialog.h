#ifndef THORSANVIL_SLACK_API_REACTIONS_H
#define THORSANVIL_SLACK_API_REACTIONS_H


#include "ThorsSlackConfig.h"
#include "API.h"

namespace ThorsAnvil::Slack::API::Dialog
{

#if 0
TODO: Need to wrap the API so it is simpler touse.
struct Open
{
    static constexpr char const* api = "https://slack.com/api/dialog.open";
    static constexpr Method method = Method::POST;
    static constexpr Scope  scope = Scope::Bot;
    using Reply = API::OK;

    dialog
};


// Response objects

// Action objects
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Dialog::Open);
#endif
}

#endif
