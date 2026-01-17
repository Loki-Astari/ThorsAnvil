#ifndef THORSANVIL_SLACK_EVENT_EVENTURLVERIFICATION_H
#define THORSANVIL_SLACK_EVENT_EVENTURLVERIFICATION_H

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{

#if 0
// ULR Verification
{
    "token":"<XXX>",
    "challenge":"n6YAXadYpiEYPctJJacfRV4Mgl5vm5Z2ha7MuAZmznDPprGivZDq",
    "type":"url_verification"
}
#endif
struct ResponseURLVerification
{
    std::string                 challenge;
};

struct EventURLVerification
{
    std::string                 token;
    std::string                 challenge;
    //std::string                 type;
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::EventURLVerification, url_verification);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::ResponseURLVerification, challenge);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EventURLVerification, token, challenge);

#endif
