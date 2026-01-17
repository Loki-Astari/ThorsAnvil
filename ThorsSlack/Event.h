#ifndef THORSANVIL_SLACK_EVENT_EVENT_H
#define THORSANVIL_SLACK_EVENT_EVENT_H


// Documentation: https://docs.slack.dev/reference/events/message.channels/

#include "ThorsSlackConfig.h"
#include "EventCallback.h"
#include "EventURLVerification.h"
#include <variant>

namespace ThorsAnvil::Slack::Event
{

using Event = std::variant<EventCallback, EventURLVerification>;

}

#endif
