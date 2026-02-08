# NisseServer Internal Documentation

Detailed architecture, threading model, coroutine mechanics, and state management for the NisseServer library.

**Source:** `third/Nisse/src/NisseServer/`

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Core Classes](#core-classes)
- [Threading & Coroutine Model](#threading--coroutine-model)
- [Connection Lifecycle](#connection-lifecycle)
- [State Management & Thread Safety](#state-management--thread-safety)
- [EventHandler Yield State Machine](#eventhandler-yield-state-machine)
- [Type Aliases](#type-aliases)
- [File Index](#file-index)

---

## Architecture Overview

```
┌──────────────────────────────────────────────────┐
│                  User Application                 │
├──────────────────────────────────────────────────┤
│                  NisseServer                      │
│   NisseServer · Pynt · Context · EventHandler     │
│   JobQueue · Store · EventHandlerLibEvent         │
├──────────────────────────────────────────────────┤
│              External Dependencies                │
│   ThorsSocket · boost::coroutine2 · libEvent      │
└──────────────────────────────────────────────────┘
```

**NisseServer** provides the transport-agnostic core: event loop, thread pool, coroutine-based cooperative multitasking, and a protocol-agnostic `Pynt` interface.

---

## Core Classes

### NisseServer

**Files:** `NisseServer.h`, `NisseServer.cpp`

Owns and coordinates three subsystems:

| Member | Type | Role |
|--------|------|------|
| `jobQueue` | `JobQueue` | Thread pool that executes connection work items |
| `store` | `Store` | Holds all live connection state (file-descriptor indexed) |
| `eventHandler` | `EventHandler` | libEvent wrapper that dispatches I/O readiness events |

**Internal Methods:**

- `createStreamJob(StreamData&)` -- builds the coroutine that manages a single client connection's lifecycle. Sets up read/write yield lambdas, performs deferred SSL handshake, then loops calling `pynt->handleRequest()` until `PyntResult::Done`.
- `createAcceptJob(ServerData&)` -- builds the coroutine for a listening socket. Loops accepting new connections and registers each via `eventHandler.add()`.

### EventHandler

**Files:** `EventHandler.h`, `EventHandler.cpp`

A C++ wrapper around libEvent. Responsibilities:

1. Maintains libEvent `event_base` and per-fd `event` objects.
2. On I/O readiness, enqueues a job on the `JobQueue` to resume the connection's coroutine.
3. After coroutine yields, interprets the yield state and requests the corresponding state change via the `Store`.
4. Runs a periodic internal timer (10ms) that calls `Store::processUpdateRequest()` on the main thread.
5. Manages soft/hard stop logic.

**Event Dispatch (Visitor Pattern):**

The `Store` holds a `std::variant<ServerData, StreamData, OwnedFD, SharedFD, TimerData>` per fd. `EventHandler::eventAction()` uses `std::visit` with `ApplyEvent`:

- `handleServerEvent` -- pending connection → resume accept coroutine
- `handleStreamEvent` -- client socket ready → resume stream coroutine
- `handleLinkStreamEvent` -- owned secondary fd ready → resume owner's coroutine
- `handlePipeStreamEvent` -- shared fd ready → resume next queued coroutine
- `handleTimerEvent` -- timer fired → call `TimerAction::handleRequest()`

### JobQueue

**Files:** `JobQueue.h`, `JobQueue.cpp`

Thread pool backed by `std::thread`, `std::mutex`, and `std::condition_variable`.

Each worker loops calling `getNextJob()` which blocks on the condition variable until work is available or `finished` is set.

### Store

**Files:** `Store.h`, `Store.cpp`

Central state repository. Every active file descriptor has an entry in `std::map<int, StoreData>`.

```cpp
using StoreData = std::variant<ServerData, StreamData, OwnedFD, SharedFD, TimerData>;
```

**Data Structures per FD:**

| Type | Contents |
|------|----------|
| `ServerData` | `TASock::Server`, `CoRoutine`, read `Event`, `Pynt*` |
| `StreamData` | `TASock::SocketStream`, `CoRoutine`, read+write `Event`, `Pynt*` |
| `OwnedFD` | Pointer to owner's `CoRoutine`, read+write `Event` |
| `SharedFD` | Queues of waiting `CoRoutine*` (read/write), read+write `Event` |
| `TimerData` | Timer ID, interval, `TimerAction*`, `EventHandler*`, timer `Event` |

**State Update Types:**

| Update Type | Action |
|-------------|--------|
| `StateUpdateCreateServer` | Insert listening socket + activate read event |
| `StateUpdateCreateStream` | Insert client stream + activate read event |
| `StateUpdateCreateOwnedFD` | Link secondary fd to owner's coroutine |
| `StateUpdateCreateSharedFD` | Create shared fd entry with empty wait queues |
| `StateUpdateCreateTimer` | Create timer entry and start timer |
| `StateUpdateExternallClosed` | Mark socket as externally closed, remove |
| `StateUpdateRemove` | Remove fd from store |
| `StateUpdateRestoreRead` | Re-register read event |
| `StateUpdateRestoreWrite` | Re-register write event |

### Context

**Files:** `Context.h`, `Context.cpp`

Interface for connection handlers to interact with the async I/O system.

**Shared Socket Registration:**

For sockets shared across multiple connections (e.g., connection pool pipes), `registerSharedSocket()` creates a `SharedFD` entry. Multiple coroutines can wait on the same shared socket; they are queued and resumed in FIFO order.

### EventHandlerLibEvent (RAII Wrappers)

**File:** `EventHandlerLibEvent.h`

- **`EventBase`** -- wraps `event_base*`. Provides `run()` and `loopBreak()`. Exposes feature detection (`isFeatureEnabled`) to check whether `epoll` supports file descriptors.
- **`Event`** -- wraps `event*`. Move-only. Provides `add()` and `add(int microseconds)`.
- **`EventType`** -- enum: `Read`, `Write`.
- **`Feature`** -- enum: `FileReadWriteEvent` (false on `epoll` systems).

---

## Threading & Coroutine Model

```
Main Thread                    Worker Threads (N)
───────────                    ──────────────────
libEvent loop                  JobQueue consumers
  │                               │
  ├─ I/O readiness event          │
  │  └─ Enqueue job ──────────►   ├─ Resume coroutine
  │                               │   ├─ Read/write on stream
  ├─ Timer tick (10ms)            │   ├─ I/O would block?
  │  └─ processUpdateRequest()    │   │  └─ Yield ──► (suspended)
  │     └─ Apply state changes    │   │     └─ Enqueue state change
  │        └─ Re-register events  │   └─ Handler returns
  │                               │      └─ Enqueue state change
  └─ (loop continues)            └─ (pick up next job)
```

**Key invariant:** All mutations to the `Store` data map and libEvent registrations happen on the main thread (via `processUpdateRequest()`). Worker threads only enqueue `StateUpdate` requests.

**Coroutine mechanics:** Each connection gets a `boost::coroutines2` pull-type coroutine. When I/O would block, ThorsSocket calls the registered yield function, which pushes a `TaskYieldAction` and suspends the coroutine. The worker thread returns to the job queue. When the event loop detects readiness, it enqueues a new job that resumes the coroutine from where it left off.

---

## Connection Lifecycle

```
1. Server socket receives connection
   └─ Accept coroutine creates SocketStream
      └─ EventHandler::add(stream, ...) enqueues StateUpdateCreateStream

2. Main thread timer fires
   └─ Store::processUpdateRequest()
      └─ Creates StreamData, stores coroutine, activates read Event

3. Client sends data → libEvent triggers read event
   └─ EventHandler::handleStreamEvent()
      └─ jobQueue.addJob() → resumes coroutine

4. Coroutine runs Pynt::handleRequest()
   ├─ Read from stream → may yield (RestoreRead)
   ├─ Write to stream  → may yield (RestoreWrite)
   └─ Returns PyntResult

5a. PyntResult::More → yield(WaitForMore)
    └─ Re-register read event, wait for next request → back to step 3

5b. PyntResult::Done → yield(Remove)
    └─ Remove from Store, close socket
```

---

## State Management & Thread Safety

The `Store` uses a **command queue pattern**:

1. **Worker threads** call `store.requestChange(update)` which locks `updateMutex` and pushes to the `updates` vector.
2. **Main thread** (via timer callback) calls `store.processUpdateRequest()` which locks `updateMutex`, drains the vector, and applies each update sequentially.
3. The `data` map is only ever modified by the main thread, so no locking is needed for reads during event dispatch.

The `openStreamCount` is incremented/decremented by worker threads but is only read by the main thread during soft-stop checks. The slight race is acceptable (the timer will re-check).

---

## EventHandler Yield State Machine

When a coroutine yields, the `EventHandler::addJob()` lambda inspects the `TaskYieldState`:

| State | Meaning | Action |
|-------|---------|--------|
| `RestoreRead` | Blocked on read; resume when readable | Re-register read event |
| `RestoreWrite` | Blocked on write; resume when writable | Re-register write event |
| `WaitForMore` | Request complete; wait for next request | Re-register read event; decrement active count |
| `Remove` | Connection done; clean up | Remove fd from store; decrement active count |

---

## Type Aliases

**File:** `NisseUtil.h`

```cpp
enum class TaskYieldState { RestoreRead, RestoreWrite, WaitForMore, Remove };

struct TaskYieldAction {
    TaskYieldState state;
    int            fd;
};

using CoRoutine     = boost::coroutines2::coroutine<TaskYieldAction>::pull_type;
using Yield         = boost::coroutines2::coroutine<TaskYieldAction>::push_type;
using ServerTask    = std::function<void(TASock::Server& stream, Yield& yield)>;
using StreamTask    = std::function<void(TASock::SocketStream& stream, Yield& yield)>;
```

---

## Header-Only Mode

When `NISSE_HEADER_ONLY` is defined to `1`:
- Implementation files use `.source` extension
- Headers conditionally `#include` the `.source` files
- `NISSE_HEADER_ONLY_INCLUDE` expands to `inline` to avoid ODR violations

---

## File Index

| File | Description |
|------|-------------|
| `NisseServer.h/.cpp` | Main server class |
| `Pynt.h/.cpp` | Abstract protocol handler |
| `PyntControl.h/.cpp` | TCP control port (hard stop) |
| `Context.h/.cpp` | Per-connection context, async socket registration |
| `EventHandler.h/.cpp` | libEvent wrapper, I/O dispatch, yield state machine |
| `EventHandlerLibEvent.h` | RAII wrappers for `event_base` and `event` |
| `JobQueue.h/.cpp` | Thread pool |
| `Store.h/.cpp` | State repository, command-queue mutations |
| `NisseUtil.h` | Core type aliases |
| `TimerAction.h` | Abstract timer callback |
