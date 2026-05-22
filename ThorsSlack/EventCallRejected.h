#ifndef THORSANVIL_SLACK_EVENT_CALL_REJECTED_H
#define THORSANVIL_SLACK_EVENT_CALL_REJECTED_H

// Documentation: https://docs.slack.dev/reference/events/call_rejected

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/call_rejected
  "event":
  {
    "type": "call_rejected",
    "call_id": "R123ABC456",
    "user_id": "U123ABC456",
    "channel_id": "D123ABC456",
    "external_unique_id": "123-456-7890"
  }
#endif

struct CallRejected
{
    // std::string                 type;
    std::string                 call_id;
    std::string                 user_id;
    std::string                 channel_id;
    std::string                 external_unique_id;
    static std::string const& typeName() {static std::string const name{"CallRejected"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::CallRejected, call_rejected);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::CallRejected, call_id, user_id, channel_id, external_unique_id);

#endif
