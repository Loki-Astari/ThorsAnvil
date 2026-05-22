#ifndef THORSANVIL_SLACK_EVENT_TOKENS_REVOKED_H
#define THORSANVIL_SLACK_EVENT_TOKENS_REVOKED_H

// Documentation: https://docs.slack.dev/reference/events/tokens_revoked

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/tokens_revoked
  "event":
  {
    "type": "tokens_revoked",
    "tokens":
    {
        "oauth": ["UXXXXXXXX"],
        "bot": ["UXXXXXXXX"]
    }
  }
#endif

using VecString = std::vector<std::string>;

struct RevokedTokens
{
    VecString                   oauth;
    VecString                   bot;
};

struct TokensRevoked
{
    // std::string                 type;
    RevokedTokens               tokens;
    static std::string const& typeName() {static std::string const name{"TokensRevoked"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::TokensRevoked, tokens_revoked);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::RevokedTokens, oauth, bot);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::TokensRevoked, tokens);

#endif
