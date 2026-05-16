#ifndef THORSANVIL_SLACK_EVENT_LINK_SHARED_H
#define THORSANVIL_SLACK_EVENT_LINK_SHARED_H

// Documentation: https://docs.slack.dev/reference/events/link_shared

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <optional>
#include <vector>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/link_shared
  "event":
  {
    "type": "link_shared",
    "channel": "C123ABC456",
    "is_bot_user_member": true,
    "user": "U123ABC456",
    "message_ts": "123456789.9875",
    "unfurl_id": "C123456.123456789.987501.1b90fa...",
    "source": "conversations_history",
    "links": [
        { "domain": "example.com", "url": "https://example.com/12345" }
    ],
    "user_locale": "en-US",
    "thread_ts": "123456621.1855",
    "is_unfurl_refresh": true
  }
#endif

using OptString = std::optional<std::string>;
using OptBool = std::optional<bool>;

struct SharedLink
{
    std::string                 domain;
    std::string                 url;
};
using VecSharedLink = std::vector<SharedLink>;

struct LinkShared
{
    // std::string                 type;
    std::string                 channel;
    bool                        is_bot_user_member;
    std::string                 user;
    std::string                 message_ts;
    std::string                 unfurl_id;
    std::string                 source;
    VecSharedLink               links;
    OptString                   user_locale;
    OptString                   thread_ts;
    OptBool                     is_unfurl_refresh;
    static std::string const& typeName() {static std::string const name{"Event/LinkShared"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::LinkShared, link_shared);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::SharedLink, domain, url);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::LinkShared, channel, is_bot_user_member, user, message_ts, unfurl_id, source, links, user_locale, thread_ts, is_unfurl_refresh);

#endif
