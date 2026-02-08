# ThorsSlack Internal Documentation

Implementation details for the ThorsSlack library.

**Source:** `third/Mug/src/ThorsSlack/`

---

## Architecture

ThorsSlack is a header-heavy library that provides type-safe C++ representations of Slack API objects. It relies heavily on ThorsSerializer for automatic JSON serialization.

```
┌──────────────────────────────────────────────────┐
│                 User Application                  │
├──────────────────────────────────────────────────┤
│                  ThorsSlack                       │
│   SlackClient · SlackEventHandler · API types     │
├──────────────────────────────────────────────────┤
│   ThorsSerializer (JSON) · NisseHTTP (webhooks)   │
└──────────────────────────────────────────────────┘
```

---

## File Organization

### Communication Layer

| File | Purpose |
|------|---------|
| `SlackClient.h` | HTTP client for making REST API calls to Slack |
| `SlackStream.h` | Stream-based communication with Slack APIs |
| `SlackEventHandler.h` | Webhook event handler for incoming Slack events |
| `SlashCommand.h` | Slash command request/response types |

### REST API Types

Each `API*.h` file defines request and response types for a group of Slack API endpoints:

| File | Slack API Methods |
|------|-------------------|
| `API.h` | Common API types |
| `APIAuth.h` | `auth.test` |
| `APIChat.h` | Common chat types |
| `APIChatMessage.h` | `chat.postMessage`, `chat.update`, `chat.delete` |
| `APIChatSchedule.h` | `chat.scheduleMessage`, `chat.scheduledMessages.list` |
| `APIChatStream.h` | Stream-based chat operations |
| `APIChatUtil.h` | Chat utility types |
| `APIConversationsHistory.h` | `conversations.history` |
| `APIDialog.h` | Dialog interactions |
| `APIPins.h` | `pins.add`, `pins.remove`, `pins.list` |
| `APIReactions.h` | `reactions.add`, `reactions.remove`, `reactions.get`, `reactions.list` |
| `APIStar.h` | `stars.add`, `stars.remove`, `stars.list` |
| `APIViews.h` | View operations |
| `APIBlockActions.h` | Block action interactions |

### Event Types

| File | Event Type |
|------|------------|
| `Event.h` | Base event types and discriminators |
| `EventCallback.h` | Event callback wrapper |
| `EventCallbackAppMentioned.h` | App mention events |
| `EventCallbackMessage.h` | Message events |
| `EventCallbackPin.h` | Pin add/remove events |
| `EventCallbackReaction.h` | Reaction add/remove events |
| `EventCallbackStar.h` | Star add/remove events |
| `EventURLVerification.h` | URL verification challenge (required by Slack) |

### UI Types

| File | Purpose |
|------|---------|
| `SlackBlockKit.h` | Block Kit UI elements (sections, actions, inputs, etc.) |

---

## Serialization Pattern

All Slack API types use ThorsSerializer macros for automatic JSON conversion:

```cpp
struct PostMessage {
    std::string channel;
    std::string text;
    // ... other fields
};
ThorsAnvil_MakeTrait(PostMessage, channel, text, ...);
```

This means:
- Sending a Slack API request = serializing a C++ struct to JSON
- Receiving a Slack API response = deserializing JSON to a C++ struct
- No manual JSON building or parsing required

---

## Event Handling Architecture

Slack sends events via HTTP POST webhooks. The event handling uses polymorphic deserialization:

1. Incoming JSON contains a `"type"` field to identify the event type.
2. ThorsSerializer's polymorphic deserialization reads the type field.
3. The correct C++ event type is instantiated and populated.
4. The `SlackEventHandler` dispatches to the appropriate callback.

### URL Verification

Slack requires URL verification when setting up event subscriptions. `EventURLVerification` handles the challenge-response protocol:
1. Slack sends `{"type": "url_verification", "challenge": "..."}`
2. The handler responds with `{"challenge": "..."}`.

---

## SlackClient

Manages authentication tokens and HTTP communication with Slack's API:
- Constructs HTTP requests with proper headers (`Authorization: Bearer <token>`)
- Serializes request body as JSON
- Deserializes response body
- Handles Slack API error responses (`"ok": false`)

---

## Integration with Mug

ThorsSlack is designed to run as a Mug plugin. A typical Slack bot:

1. Creates a `MugPluginSimple` subclass
2. Registers POST routes for Slack webhook URLs
3. In the handler, deserializes the Slack event using ThorsSerializer
4. Processes the event (e.g., responds to messages)
5. Uses `SlackClient` to make outbound API calls

---

## Test Infrastructure

Tests are in `test/` and use `Environment.h` to provide test OAuth tokens and channel IDs. Tests verify serialization round-trips for all API types.
