#ifndef THORSANVIL_SLACK_EVENT_EMAIL_DOMAIN_CHANGED_H
#define THORSANVIL_SLACK_EVENT_EMAIL_DOMAIN_CHANGED_H

// Documentation: https://docs.slack.dev/reference/events/email_domain_changed

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/email_domain_changed
  "event":
  {
    "type": "email_domain_changed",
    "email_domain": "example.com",
    "event_ts": "1360782804.083113"
  }
#endif

struct EmailDomainChanged
{
    // std::string                 type;
    std::string                 email_domain;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"Event/EmailDomainChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::EmailDomainChanged, email_domain_changed);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::EmailDomainChanged, email_domain, event_ts);

#endif
