#ifndef THORSANVIL_SLACK_API_CHAT_H
#define THORSANVIL_SLACK_API_CHAT_H


#include "ThorsSlackConfig.h"
#include "API.h"

namespace ThorsAnvil::Slack::API::Chat
{

struct Payload
{
    std::string                 id;
    std::string                 title;
};

struct Metadata
{
    std::string                 event_type;
    Payload                     event_payload;
};
using OptMetadata = std::optional<Metadata>;

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::Payload, id, title);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::API::Chat::Metadata, event_type, event_payload);

#endif
