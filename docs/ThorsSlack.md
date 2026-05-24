[Home](index.html) | [Internal Documentation](internal/ThorsSlack.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseBolt](NisseBolt.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsMongo](ThorsMongo.html) · [ThorsLogging](ThorsLogging.html)

---

# ThorsSlack API

Type-safe C++ API for the Slack platform. Provides strongly-typed interfaces for the Slack REST API, webhooks, event callbacks, slash commands, and Block Kit -- with automatic JSON serialization via ThorsSerializer.

**Namespace:** `ThorsAnvil::Slack` (within the NisseBolt repository)

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorsSlack/Client.h` | HTTP client for Slack REST API calls |
| `ThorsSlack/Stream.h` | Stream-based Slack communication |
| `ThorsSlack/EventHandler.h` | Webhook event handler |
| `ThorsSlack/SlashCommand.h` | Slash command support |
| `ThorsSlack/BlockKit.h` | Block Kit UI elements |
| `ThorsSlack/API.h` | Common API types |

---

## REST API Modules

ThorsSlack provides type-safe wrappers for Slack API endpoints. Each module is a header that defines the request/response types with automatic JSON serialization:

| Header | Slack API |
|--------|-----------|
| `APIAuth.h` | Authentication (`auth.test`) |
| `APIChat.h` | Chat operations |
| `APIChatMessage.h` | `chat.postMessage`, `chat.update`, `chat.delete` |
| `APIChatSchedule.h` | `chat.scheduleMessage`, `chat.scheduledMessages.list` |
| `APIChatStream.h` | Stream-based chat |
| `APIChatUtil.h` | Chat utility types |
| `APIConversationsHistory.h` | `conversations.history` |
| `APIPins.h` | `pins.add`, `pins.remove`, `pins.list` |
| `APIReactions.h` | `reactions.add`, `reactions.remove`, `reactions.get`, `reactions.list` |
| `APIStar.h` | `stars.add`, `stars.remove`, `stars.list` |
| `APIViews.h` | View operations |
| `APIBlockActions.h` | Block action interactions |

---

## Event Handling

Handle incoming Slack events via webhook:

| Header | Event Type |
|--------|------------|
| `Event.h` | Base event types |
| `EventCallback.h` | Event callback wrapper |
| `EventAppMentioned.h` | App mention events |
| `EventMessage.h` | Message events |
| `EventsPin.h` | Pin events |
| `EventsReaction.h` | Reaction events |
| `EventsStar.h` | Star events |
| `EventURLVerification.h` | URL verification challenge |

---

## Block Kit

`BlockKit.h` provides C++ types for Slack's Block Kit UI framework, allowing you to build rich message layouts with type safety.

---

## Integration with Mug

ThorsSlack is designed to be used as a Mug plugin. Create a `MugPlugin` that uses ThorsSlack types to handle Slack webhook events and API calls:

```cpp
#include "ThorsMug/MugPlugin.h"
#include "ThorsSlack/EventHandler.h"
#include "ThorsSlack/EventMessage.h"

class SlackPlugin : public ThorsAnvil::ThorsMug::MugPluginSimple
{
public:
    std::vector<ThorsAnvil::ThorsMug::Action> getAction() override
    {
        return {
            {NisHttp::Method::POST, "/slack/events",
                [](NisHttp::Request const& req, NisHttp::Response& res) {
                    // Parse and handle Slack event using ThorsSlack types
                    // ThorsSerializer automatically deserializes the JSON payload
                    return true;
                }
            }
        };
    }
};
```

All request and response types are declared as serializable via `ThorsAnvil_MakeTrait`, so they work directly with `jsonImporter()` and `jsonExporter()`.
