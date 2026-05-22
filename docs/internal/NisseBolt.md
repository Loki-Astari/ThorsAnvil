[Home](../index.html) | [API Documentation](../NisseBolt.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseBolt](NisseBolt.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsMongo](ThorsMongo.html) · [ThorsLogging](ThorsLogging.html)

---

# NisseBolt Internal Documentation

Detailed implementation of the Bolt-style Slack bot framework, covering the standalone server (`NB`) and the Mug plugin adapter (`NisseBolt`).

**Source:** `third/NisseBolt/src/NB/` (standalone server) and `third/NisseBolt/src/NisseBolt/` (Mug plugin)

---

## Table of Contents

- [Architecture](#architecture)
- [Two Deployment Modes](#two-deployment-modes)
- [Standalone Server (NB)](#standalone-server-nb)
- [Mug Plugin Mode (NisseBolt)](#mug-plugin-mode-nissebolt)
- [Request Dispatch Pipeline](#request-dispatch-pipeline)
- [Handler Type Aliases](#handler-type-aliases)
- [ThorsSlack Integration](#thorsslack-integration)
- [File Index](#file-index)

---

## Architecture

```
┌──────────────────────────────────────────────────┐
│              User Bot Application                 │
├──────────────────────────────────────────────────┤
│                   NisseBolt                       │
│   NBServer · App · Ack · Say · Respond · View     │
├──────────────────────────────────────────────────┤
│                   ThorsSlack                      │
│      Client · EventHandler · BlockKit             │
├──────────────────────────────────────────────────┤
│          NisseHTTP · NisseServer · ThorsSocket     │
└──────────────────────────────────────────────────┘
```

NisseBolt sits between the user's bot logic and the ThorsSlack/Nisse stack. It provides Bolt-style handler registration (similar to Slack's Node.js Bolt framework) while delegating protocol parsing to `EventHandler` and async I/O to `NisseServer`.

---

## Two Deployment Modes

### 1. Standalone Server (`NB::NBServer`)

A self-contained executable. `NBServer` inherits from both `NisseServer` (event loop) and `EventHandler` (Slack protocol). The user constructs it with an `NBConfig`, registers handlers, and calls `run()`.

### 2. Mug Plugin (`NisseBolt::App`)

A shared library loaded by the Mug server. `App` inherits from `MugPluginSimple` and registers three HTTP routes (events, slash commands, user actions) via `getAction()`. Multiple bots can coexist in the same Mug process, each occupying a unique `slot` URL prefix.

---

## Standalone Server (NB)

### NBConfig

**File:** `NB/NBConfig.h`

Serializable configuration struct. Deserialized from JSON using `ThorsAnvil_MakeTrait`:

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `port` | `int` | required | Listening port (0 = test mode, no socket) |
| `domainName` | `string` | required | Domain for TLS cert lookup |
| `certRoot` | `string` | `""` | TLS certificate root directory |
| `botToken` | `string` | required | Slack bot token (`xoxb-...`) |
| `signingSecret` | `string` | required | HMAC signing secret |
| `basePath` | `string` | `"/slack"` | URL prefix for the four Slack routes |

### NBServer

**Files:** `NB/NBServer.h`, `NB/NBServer.cpp`

Dual-inherits `NisseServer` and `Slack::EventHandler`.

**Construction sequence:**
1. Initializes `NisseServer` with 4 worker threads.
2. Initializes `EventHandler` with the signing secret.
3. Creates a `Client` with the bot token.
4. Calls `registerRoutes()` to mount the four Slack endpoints on the internal `HTTPHandler`.
5. If `port != 0`, resolves TLS certificates and calls `listen()`.

**Route registration (`registerRoutes`):**

All routes use the same `HTTPValidate` callback that delegates to `EventHandler::validateRequest()` for HMAC signature verification.

| Route | Handler |
|-------|---------|
| `POST {basePath}/events` | `EventHandler::handleEvent()` |
| `POST {basePath}/commands` | `EventHandler::handleSlashCommand()` |
| `POST {basePath}/actions` | `EventHandler::handleUserActions()` |

**Slash command override:**

`NBServer` overrides `EventHandler::handleSlashWithCommand()` to inject the Bolt dispatch layer:
1. Looks up the command in `commandHandlers`.
2. Runs middleware chain; short-circuits on `false`.
3. Constructs `Ack`, `Respond`, and `Context` objects.
4. Invokes the registered handler inside a try/catch.
5. On exception: suppresses auto-ack and invokes `errorHandler` (or returns 500).

### Handler Storage

| Member | Type | Key |
|--------|------|-----|
| `commandHandlers` | `unordered_map<string, CommandHandler>` | command name (e.g. `/deploy`) |
| `messageListeners` | `vector<MessageMatcher>` | sequential match (substring or regex) |
| `actionHandlers` | `unordered_map<string, ActionHandler>` | `action_id` |
| `shortcutHandlers` | `unordered_map<string, ShortcutHandler>` | `callback_id` |
| `viewHandlers` | `unordered_map<string, ViewHandler>` | view `callback_id` |
| `optionsHandlers` | `unordered_map<string, OptionsHandler>` | `action_id` |
| `middleware` | `vector<Middleware>` | ordered chain |

### MessageMatcher

```cpp
struct MessageMatcher {
    std::regex      pattern;
    MessageHandler  handler;
    bool            isRegex;
    std::string     substring;
};
```

When `isRegex` is `false`, dispatch checks `event.text.find(substring) != npos`. When `true`, it uses `std::regex_search`.

---

## Mug Plugin Mode (NisseBolt)

### AppConfig

**File:** `NisseBolt/AppConfig.h`

| Field | Description |
|-------|-------------|
| `botToken` | Slack bot token |
| `userToken` | Slack user token |
| `signingSecret` | HMAC signing secret |

The `slot` (URL prefix) is now passed as a constructor parameter to `App` rather than being part of the config.

### App

**Files:** `NisseBolt/App.h`, `NisseBolt/App.cpp`

Inherits from `MugPluginSimple`. Owns a `Client`, `EventHandler`, and handler maps for events, slash commands, actions, and views.

**Construction:**
1. Stores the `slot` for URL prefix routing.
2. Creates `Client` with bot and user tokens.
3. Creates `EventHandler` with the signing secret and all handler maps.
4. Calls `addEventHandlers()` to register all known Slack event types.

**`getAction()` returns three routes:**

| Route | Handler |
|-------|---------|
| `POST {slot}/event` | `slackHandler.handleEvent()` |
| `POST {slot}/slash/{CommandString}` | `slackHandler.handleSlashCommand()` |
| `POST {slot}/useraction` | `slackHandler.handleUserActions()` |

All routes include a validation callback for HMAC signature checking.

### Bot Lifecycle (Mug)

The `THORS_ANVIL_NISSE_BOLT_SERVER_INIT(Config, Server)` macro exports:

```cpp
extern "C" MugPlugin* mugCreateInstance(int init, char const* configStr)
```

This calls `mugCreateBoltInstance<Server, Config>()` which:
- **init=1:** Deserializes JSON config, creates a new `App` in the static `getServerInfo()` map keyed by `slot`. Throws if slot is already occupied.
- **init=0:** Removes the `App` from the map (sets the `unique_ptr` to null).

Multiple bots can coexist because each occupies a unique `slot` prefix.

---

## Request Dispatch Pipeline

### Event Messages (App mode)

```
Slack webhook POST → EventHandler::handleEvent()
  → eventHandlerMap[typeName] → App::handleEventMessage()
    → for each (filter, handler) in messageHandlers:
        if filter(message):
            handler(event, Say{client, Where{channel, ts}})
```

### Slash Commands (NBServer mode)

```
Slack POST /commands → EventHandler::handleSlashCommand()
  → NBServer::handleSlashWithCommand() [override]
    → middleware chain
    → commandHandlers[command.command](command, ack, respond, ctx)
```

---

## Handler Type Aliases

### NBServer (Standalone)

```cpp
using CommandHandler  = function<void(SlashCommand const&, Ack&, Respond&, Context&)>;
using MessageHandler  = function<void(Event::EventCallback const&, Ack&, Say&, Context&)>;
using ActionHandler   = function<void(API::BlockActions const&, Ack&, Respond&, Context&)>;
using ShortcutHandler = function<void(API::BlockActions const&, Ack&, Context&)>;
using ViewHandler     = function<void(API::Views::ViewSubmission const&, Ack&, Context&)>;
using OptionsHandler  = function<void(API::BlockActions const&, Ack&, Context&)>;
using Middleware       = function<bool(Context&)>;
using ErrorHandler    = function<void(exception const&, Context&)>;
```

### App (Mug Plugin)

```cpp
using Filter              = function<bool(Event::Message const&)>;
using MessageRunner       = EventRunner<Event::Message>;
using SlashCommandRunner  = function<void(Ack const&, Response const&, SlashCommand const&)>;
using ActionRunner        = function<void(Ack const&, Response const&, API::BlockActions const&, string const& value)>;
using ViewSubmitRunner    = function<void(Ack const&, Response const&, API::Views::ViewSubmission const&)>;
using ViewClosedRunner    = function<void(Ack const&, Response const&, API::Views::ViewClosed const&)>;
```

---

## ThorsSlack Integration

NisseBolt delegates all Slack protocol concerns to the ThorsSlack library:

| Concern | ThorsSlack Class |
|---------|-----------------|
| HMAC request verification | `EventHandler::validateRequest()` |
| Event payload parsing & dispatch | `EventHandler::handleEvent()` |
| Slash command parsing | `EventHandler::handleSlashCommand()` |
| Action/view interaction parsing | `EventHandler::handleUserActions()` |
| REST API calls (chat.postMessage, views.open, etc.) | `Client::sendMessage()` |
| Block Kit UI types | `BlockKit.h` |

### Say Implementation

`Say` wraps `Client::sendMessage()` to send `chat.postMessage` calls. It auto-populates `channel` and `thread_ts` from the originating event's `Where` struct. Supports plain text and Block Kit messages.

### Ack and SlashAck

`Ack` writes an HTTP response to acknowledge receipt within Slack's 3-second window. `SlashAck` extends `Ack` to support JSON body responses with `Content-Type: application/json` for slash command feedback.

### View Management (App mode)

`App::viewOpen()` and `App::viewPush()` call `Client::sendMessage()` with `Views::Open` / `Views::Push` payloads and register the returned `view.id` in `viewHandlerMap` for subsequent `view_submission` events. `viewPush()` also tracks `previous_view_id` for view stack navigation.

---

## File Index

### Standalone Server (`src/NB/`)

| File | Purpose |
|------|---------|
| `NB.cpp` | `main()` entry point: reads config, constructs `NBServer`, runs |
| `NBConfig.h` | `NBConfig` struct with ThorsSerializer traits |
| `NBServer.h` | `NBServer` class declaration with all handler types |
| `NBServer.cpp` | Construction, route registration, slash command dispatch, listener storage |
| `NBContext.h` | Per-request context (team, user, channel, client access) |

### Mug Plugin (`src/NisseBolt/`)

| File | Purpose |
|------|---------|
| `App.h` | `App` class, `mugCreateBoltInstance()` template, `THORS_ANVIL_NISSE_BOLT_SERVER_INIT` macro |
| `App.cpp` | Construction, event handler registration, message/command/action/view dispatch |
| `AppConfig.h` | `AppConfig` struct with ThorsSerializer traits |
| `Runners.h` | Type aliases for all runner function signatures, `AnyEventRunner` variant |
| `Say.h` / `Say.cpp` | `Say` class: sends messages via `Client` |
| `Ack.h` / `Ack.cpp` | `Ack` and `SlashAck`: HTTP response acknowledgement |
| `Response.h` | `Response` placeholder struct |
| `View.h` / `View.cpp` | `View` class: modal display + submit/close handlers |
| `NisseBoltConfig.h` | Build configuration (generated by configure) |

### ThorsSlack (`src/ThorsSlack/`)

| File | Purpose |
|------|---------|
| `Client.h` | HTTP client for Slack REST API calls |
| `EventHandler.h` | HMAC verification, event/command/action dispatch |
| `BlockKit.h` | Block Kit UI element types |
| `SlashCommand.h` | Slash command payload types |
| `API*.h` | Typed Slack API request/response structs |
| `Event*.h` | Typed event callback payload structs |
