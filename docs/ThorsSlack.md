[Home](index.html) | [Internal Documentation](internal/ThorsSlack.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsLogging](ThorsLogging.html) · [ThorsIOUtil](ThorsIOUtil.html)

---

# ThorsSlack API

Type-safe C++ API for the Slack platform. Provides strongly-typed interfaces for the Slack REST API, webhooks, event callbacks, slash commands, and Block Kit -- with automatic JSON serialization via ThorsSerializer.

**Namespace:** `ThorsAnvil::ThorsMug` (within the Mug repository)

---

## Headers

| Header | Purpose |
|--------|---------|
| `ThorsSlack/SlackClient.h` | HTTP client for Slack REST API calls |
| `ThorsSlack/SlackStream.h` | Stream-based Slack communication |
| `ThorsSlack/SlackEventHandler.h` | Webhook event handler |
| `ThorsSlack/SlashCommand.h` | Slash command support |
| `ThorsSlack/SlackBlockKit.h` | Block Kit UI elements |
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
| `APIDialog.h` | Dialog interactions |
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
| `EventCallbackAppMentioned.h` | App mention events |
| `EventCallbackMessage.h` | Message events |
| `EventCallbackPin.h` | Pin events |
| `EventCallbackReaction.h` | Reaction events |
| `EventCallbackStar.h` | Star events |
| `EventURLVerification.h` | URL verification challenge |

---

## Block Kit

`SlackBlockKit.h` provides C++ types for Slack's Block Kit UI framework, allowing you to build rich message layouts with type safety.

---

## Integration with Mug

ThorsSlack is designed to be used as a Mug plugin. Create a `MugPlugin` that uses ThorsSlack types to handle Slack webhook events and API calls:

```cpp
#include "ThorsMug/MugPlugin.h"
#include "ThorsSlack/SlackEventHandler.h"
#include "ThorsSlack/EventCallbackMessage.h"

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
