#ifndef THORSANVIL_SLACK_EVENT_ASSISTANT_THREAD_H
#define THORSANVIL_SLACK_EVENT_ASSISTANT_THREAD_H

// Documentation: https://docs.slack.dev/reference/events/assistant_thread_started

#include "ThorsSlackConfig.h"
#include "ThorSerialize/Traits.h"
#include <string>
#include <optional>

namespace ThorsAnvil::Slack::Event
{
#if 0
// https://docs.slack.dev/reference/events/assistant_thread_context_changed
  "event":
  {
    "type": "assistant_thread_context_changed",
    "assistant_thread":
    {
        "user_id": "U123ABC456",
        "context":
        {
            "channel_id": "C123ABC456",
            "team_id": "T07XY8FPJ5C",
            "enterprise_id": "E480293PS82"
        },
        "channel_id": "D123ABC456",
        "thread_ts": "1729999327.187299"
    },
    "event_ts": "17298244.022142"
  }
// https://docs.slack.dev/reference/events/assistant_thread_started
  "event":
  {
    "type": "assistant_thread_started",
    "assistant_thread":
    {
        "user_id": "U123ABC456",
        "context":
        {
            "channel_id": "C123ABC456",
            "team_id": "T07XY8FPJ5C",
            "enterprise_id": "E480293PS82"
        },
        "channel_id": "D123ABC456",
        "thread_ts": "1729999327.187299"
    },
    "event_ts": "1715873754.429808"
  }
#endif

using OptString = std::optional<std::string>;

struct AssistantThreadContext
{
    std::string                 channel_id;
    std::string                 team_id;
    OptString                   enterprise_id;
};

struct AssistantThread
{
    std::string                 user_id;
    AssistantThreadContext       context;
    std::string                 channel_id;
    std::string                 thread_ts;
};

struct AssistantThreadContextChanged
{
    // std::string                 type;
    AssistantThread              assistant_thread;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"AssistantThreadContextChanged"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AssistantThreadContextChanged, assistant_thread_context_changed);
    ThorsAnvil_TypeFieldName(type);
};

struct AssistantThreadStarted
{
    // std::string                 type;
    AssistantThread              assistant_thread;
    std::string                 event_ts;
    static std::string const& typeName() {static std::string const name{"AssistantThreadStarted"}; return name;}
    ThorsAnvil_VariantSerializerWithName(ThorsAnvil::Slack::Event::AssistantThreadStarted, assistant_thread_started);
    ThorsAnvil_TypeFieldName(type);
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AssistantThreadContext, channel_id, team_id, enterprise_id);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AssistantThread, user_id, context, channel_id, thread_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AssistantThreadContextChanged, assistant_thread, event_ts);
ThorsAnvil_MakeTrait(ThorsAnvil::Slack::Event::AssistantThreadStarted, assistant_thread, event_ts);

#endif
