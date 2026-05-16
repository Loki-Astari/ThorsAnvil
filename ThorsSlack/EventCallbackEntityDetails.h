#ifndef THORSANVIL_SLACK_EVENT_ENTITY_DETAILS_REQUESTED_H
#define THORSANVIL_SLACK_EVENT_ENTITY_DETAILS_REQUESTED_H

// Documentation: https://docs.slack.dev/reference/events/entity_details_requested

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/entity_details_requested
  "event":
  {
    "type": "entity_details_requested",
    "user": "U0123456",
    "external_ref":
    {
        "id": "123",
        "type": "my-type"
    },
    "entity_url": "https://example.com/document/123",
    "link":
    {
        "url": "https://example.com/document/123",
        "domain": "example.com"
    },
    "app_unfurl_url": "https://example.com/document/123?myquery=param",
    "event_ts": "123456789.1234566",
    "trigger_id": "1234567890123.1234567890123.abcdef01234567890abcdef012345689",
    "user_locale": "en-US",
    "channel": "C123ABC456",
    "message_ts": "1755035323.759739",
    "thread_ts": "1755035323.759739"
  }
#endif

using OptString = std::optional<std::string>;

struct EntityExternalRef
{
    std::string                 id;
    std::string                 type;
};

struct EntityLink
{
    std::string                 url;
    std::string                 domain;
};

struct EntityDetailsRequested
{
    // std::string                 type;
    std::string                 user;
    EntityExternalRef           external_ref;
    std::string                 entity_url;
    EntityLink                  link;
    std::string                 app_unfurl_url;
    std::string                 event_ts;
    std::string                 trigger_id;
    std::string                 user_locale;
    std::string                 channel;
    std::string                 message_ts;
    OptString                   thread_ts;
    static std::string const& typeName() {static std::string const name{"Event/EntityDetailsRequested"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::EntityDetailsRequested, entity_details_requested);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EntityExternalRef, id, type);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EntityLink, url, domain);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EntityDetailsRequested, user, external_ref, entity_url, link, app_unfurl_url, event_ts, trigger_id, user_locale, channel, message_ts, thread_ts);

#endif
