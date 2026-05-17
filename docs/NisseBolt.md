[Home](index.html) | [Internal Documentation](internal/NisseBolt.html)

**Libraries:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseBolt](NisseBolt.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsMongo](ThorsMongo.html) · [ThorsLogging](ThorsLogging.html)

---

# NisseBolt API

A Bolt-style application framework for building Slack bots in C++. NisseBolt provides a high-level, declarative API for handling Slack events, slash commands, interactive actions, shortcuts, views, and options -- all running on the NisseServer async framework with automatic request signing verification via ThorsSlack.

**Namespace:** `ThorsAnvil::NisseBolt`

---

## Quick Start

### Makefile
```Makefile
CXXFLAGS        = -std=c++20

LDLIBS          = -lNisseBolt -lThorsSlack -lNisse -lThorsSocket \
                  -lThorSerialize -lThorsLogging \
                  -lboost_coroutine -lboost_context -levent -lcrypto -lssl

all:            MyBot
```

### bot.config
```json
{
    "port": 3000,
    "domainName": "example.com",
    "certRoot": "/etc/letsencrypt/live",
    "botToken": "xoxb-your-bot-token",
    "signingSecret": "your-signing-secret",
    "basePath": "/slack"
}
```

### MyBot.cpp
```cpp
#include "NB/NBServer.h"
#include "NB/NBConfig.h"
#include "ThorSerialize/JsonThor.h"

#include <fstream>
#include <iostream>

namespace NB = ThorsAnvil::NisseBolt;
namespace TS = ThorsAnvil::Serialize;

int main()
{
    std::ifstream configFile("bot.config");
    NB::NBConfig config = TS::jsonBuilder<NB::NBConfig>(configFile);

    NB::NBServer server(config);

    // Respond to messages containing "hello"
    server.message("hello", [](auto const& event, auto& ack, auto& say, auto& ctx) {
        ack();
        say("Hello there!");
    });

    // Handle the /deploy slash command
    server.command("/deploy", [](auto const& command, auto& ack, auto& respond, auto& ctx) {
        ack();
        respond("Deploying " + command.text + "...");
    });

    // Handle a button click
    server.action("approve_button", [](auto const& action, auto& ack, auto& respond, auto& ctx) {
        ack();
        respond("Approved!");
    });

    std::cout << "Bot listening on port " << config.port << "\n";
    server.run();
}
```

### Testing the Bot

```bash
> make
> ./MyBot &

# Slack sends events to https://example.com/slack/events
# Slash commands hit https://example.com/slack/commands
# Interactive actions hit https://example.com/slack/actions
```

---

## Headers

| Header | Purpose |
|--------|---------|
| `NB/NBServer.h` | Main `NBServer` class with Bolt-style listener registration |
| `NB/NBConfig.h` | Configuration struct (deserialized from JSON) |
| `NB/NBContext.h` | Per-request context passed to all handlers |

---

## NBConfig

Configuration for the server, deserialized automatically from JSON via ThorsSerializer:

```cpp
struct NBConfig {
    int         port;           // Port to listen on
    std::string domainName;     // Domain name for TLS certificate lookup
    std::string certRoot;       // Root directory for TLS certificates
    std::string botToken;       // Slack bot token (xoxb-...)
    std::string signingSecret;  // Slack signing secret for HMAC verification
    std::string basePath;       // Mount point for Slack routes (default: "/slack")
};
```

If `certRoot` is empty, the server listens on plain HTTP. If `port` is 0, no socket is bound (useful for testing).

---

## NBServer

The central server class. Inherits from `NisseServer` (async event loop) and `SlackEventHandler` (request signing and payload parsing).

### Construction

```cpp
NB::NBServer server(config);   // 4 worker threads by default
```

### Running

```cpp
server.run();   // blocks until stopped
```

---

## Listener Registration

### Messages

Subscribe to Slack message events, optionally filtered by substring or regex:

```cpp
// Match messages containing "help"
server.message("help", [](auto const& event, auto& ack, auto& say, auto& ctx) {
    ack();
    say("How can I help?");
});

// Match messages with a regex
server.message(std::regex{"deploy\\s+(\\w+)"}, [](auto const& event, auto& ack, auto& say, auto& ctx) {
    ack();
    say("Starting deployment...");
});
```

**Handler signature:**
```cpp
void(Slack::Event::EventCallback const& event, Ack& ack, Say& say, Context& ctx)
```

### Slash Commands

Handle slash commands registered in your Slack app:

```cpp
server.command("/status", [](auto const& command, auto& ack, auto& respond, auto& ctx) {
    ack();
    respond("All systems operational. Requested by: " + command.user_id);
});
```

The `command` parameter includes the leading slash. The handler receives the full `SlashCommand` payload with `text`, `user_id`, `channel_id`, `team_id`, and `response_url`.

**Handler signature:**
```cpp
void(Slack::SlashCommand const& command, Ack& ack, Respond& respond, Context& ctx)
```

### Actions

Handle Block Kit interactive component actions, keyed by `action_id`:

```cpp
server.action("approve_btn", [](auto const& action, auto& ack, auto& respond, auto& ctx) {
    ack();
    respond("Request approved.");
});
```

**Handler signature:**
```cpp
void(Slack::API::BlockActions const& action, Ack& ack, Respond& respond, Context& ctx)
```

### Shortcuts

Handle global and message shortcuts, keyed by `callback_id`:

```cpp
server.shortcut("create_ticket", [](auto const& action, auto& ack, auto& ctx) {
    ack();
    // Open a modal, create a ticket, etc.
});
```

**Handler signature:**
```cpp
void(Slack::API::BlockActions const& action, Ack& ack, Context& ctx)
```

### Views

Handle modal `view_submission` and `view_closed` events, keyed by view `callback_id`:

```cpp
server.view("feedback_modal", [](auto const& submission, auto& ack, auto& ctx) {
    ack();
    // Process submitted form data
});
```

**Handler signature:**
```cpp
void(Slack::API::Views::ViewSubmission const& submission, Ack& ack, Context& ctx)
```

### Options

Handle external-select option-loading requests, keyed by `action_id`:

```cpp
server.options("user_search", [](auto const& action, auto& ack, auto& ctx) {
    ack();
    // Return dynamic options
});
```

**Handler signature:**
```cpp
void(Slack::API::BlockActions const& action, Ack& ack, Context& ctx)
```

### Generic Events

Subscribe to any typed Slack event:

```cpp
server.event<Slack::Event::AppMentioned>([](auto const& event, auto& ack, auto& say, auto& ctx) {
    ack();
    say("You mentioned me!");
});
```

---

## Cross-Cutting Concerns

### Middleware

Register middleware that runs before every handler. Return `false` to short-circuit:

```cpp
server.use([](auto& ctx) {
    // Authentication, rate-limiting, logging, etc.
    return true;  // continue to handler
});
```

Middleware executes in registration order. If any middleware returns `false`, the request is acknowledged and no handler is invoked.

### Error Handling

Register a global error handler for uncaught exceptions in handlers:

```cpp
server.onError([](std::exception const& e, auto& ctx) {
    std::cerr << "Error: " << e.what() << "\n";
});
```

If no error handler is registered, the server responds with HTTP 500.

---

## Utility Types

### Ack

Acknowledges a Slack request within the 3-second window. Call it early in your handler:

```cpp
ack();          // 200 OK with empty body
ack(200);       // explicit status code
```

### Say

Sends a message to the channel where the event originated:

```cpp
say("Hello!");                          // plain text
say(blocks);                            // Block Kit blocks
say("Hello!", Where{.channel = "C123"}); // override destination
```

### Respond

Sends a response via the `response_url` provided by Slack (for slash commands and actions):

```cpp
respond("Deployment complete.");
```

### Context

Per-request context providing access to the Slack client and request metadata:

```cpp
ctx.client();      // SlackClient for making API calls
ctx.teamId();      // team_id from the request
ctx.userId();      // user_id from the request
ctx.channelId();   // channel_id from the request
```

---

## Routes

NBServer mounts four routes under `config.basePath` (default `/slack`):

| Route | Purpose |
|-------|---------|
| `POST /slack/events` | Events API (message events, app_mention, etc.) and URL verification |
| `POST /slack/commands` | Slash command payloads |
| `POST /slack/actions` | Interactive actions, shortcuts, view submissions |
| `POST /slack/options` | External-select option loading |

All routes validate the `X-Slack-Signature` header using the configured `signingSecret`.

---

## TLS Configuration

If `certRoot` and `domainName` are provided, NBServer looks for TLS certificates at:

```
{certRoot}/{domainName}/fullchain.pem
{certRoot}/{domainName}/privkey.pem
```

If `certRoot` is empty, the server listens on plain HTTP (suitable for development or when behind a TLS-terminating reverse proxy).

---

## Mug Plugin Mode

NisseBolt also provides an `App` class for use as a Mug plugin (hot-loadable shared library). This is an alternative deployment mode where Mug manages the server lifecycle:

```cpp
#include "NisseBolt/App.h"

struct MyConfig : ThorsAnvil::Nisse::Bolt::AppConfig {
    // additional config fields
};

class MyBot : public ThorsAnvil::Nisse::Bolt::App {
public:
    MyBot(MyConfig const& config) : App(config) {
        message("hello", [](auto const& event, auto const& say) {
            say("Hi there!");
        });

        command("/status", [](auto const& ack, auto const& response, auto const& cmd) {
            ack();
        });
    }
};

THORS_ANVIL_NISSE_BOLT_SERVER_INIT(MyConfig, MyBot)
```

The `THORS_ANVIL_NISSE_BOLT_SERVER_INIT` macro exports the `mugCreateInstance` factory function expected by Mug.

### Plugin AppConfig

```cpp
struct AppConfig {
    std::string slot;           // URL path prefix for this bot's routes
    std::string botToken;       // Slack bot token
    std::string userToken;      // Slack user token
    std::string signingSecret;  // Slack signing secret
};
```

---

## Complete Example: Echo Bot

```cpp
#include "NB/NBServer.h"
#include "NB/NBConfig.h"
#include "ThorSerialize/JsonThor.h"

#include <fstream>
#include <iostream>

namespace NB = ThorsAnvil::NisseBolt;
namespace TS = ThorsAnvil::Serialize;

int main()
{
    std::ifstream configFile("bot.config");
    NB::NBConfig config = TS::jsonBuilder<NB::NBConfig>(configFile);

    NB::NBServer server(config);

    // Echo every message back
    server.message("", [](auto const& event, auto& ack, auto& say, auto& ctx) {
        ack();
        say("You said: " + event.text);
    });

    // Slash command: /echo <text>
    server.command("/echo", [](auto const& command, auto& ack, auto& respond, auto& ctx) {
        ack();
        respond(command.text);
    });

    // Middleware: log every request
    server.use([](auto& ctx) {
        std::cout << "Request from user: " << ctx.userId() << "\n";
        return true;
    });

    // Global error handler
    server.onError([](std::exception const& e, auto& ctx) {
        std::cerr << "Bot error: " << e.what() << "\n";
    });

    std::cout << "Echo bot running on port " << config.port << "\n";
    server.run();
}
```
