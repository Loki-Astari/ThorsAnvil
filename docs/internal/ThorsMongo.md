[Home](../index.html) | [API Documentation](../ThorsMongo.html)

**Internal:** [Mug](Mug.html) · [ThorsMug](ThorsMug.html) · [ThorsSlack](ThorsSlack.html) · [NisseBolt](NisseBolt.html) · [NisseServer](NisseServer.html) · [NisseHTTP](NisseHTTP.html) · [ThorsSocket](ThorsSocket.html) · [ThorsCrypto](ThorsCrypto.html) · [ThorsSerializer](ThorsSerialize.html) · [ThorsMongo](ThorsMongo.html) · [ThorsLogging](ThorsLogging.html)

---

# ThorsMongo Internal Documentation

Detailed architecture, message flow, cursor/range mechanics, and class-level documentation for the ThorsMongo MongoDB client.

**Source:** `third/ThorsMongo/src/ThorsMongo/`

---

## Table of Contents

- [File Map](#file-map)
- [Architecture Overview](#architecture-overview)
- [Public Facade Types (Thin Handles)](#public-facade-types-thin-handles)
- [Connection and Streams](#connection-and-streams)
- [MessageHandler and Message Framing](#messagehandler-and-message-framing)
- [Command Objects (Insert/Find/Remove/etc.)](#command-objects-insertfindremoveetc)
- [Cursor-backed Ranges](#cursor-backed-ranges)
- [Filters (MongoQuery) and Updates (MongoUpdate)](#filters-mongoquery-and-updates-mongoupdate)
- [Config Objects and Error Model](#config-objects-and-error-model)
- [Concerns and Preferences](#concerns-and-preferences)
- [Endianness and BSON](#endianness-and-bson)
- [Header-Only Mode](#header-only-mode)
- [Testing and Mocking](#testing-and-mocking)

---

## File Map

| File | Purpose |
|------|---------|
| `ThorsMongo.h/.cpp` | Public entry point (`ThorsMongo`, `DB`, `Collection`) and connection-owned cursor helpers (`getMore()`, `killCursor()`). |
| `ConnectionMongo.h/.cpp` | `ConnectionMongo` stream wrapper around a MongoDB TCP connection (built on ThorsSocket iostreams). |
| `ConnectionBufferMongo.h/.cpp` | `std::streambuf` implementation tuned for Mongo wire protocol framing and buffering. |
| `MessageHandler.h/.cpp` | `MessageHandler` base + `MongoActionWriteInterface` / `MongoActionReadInterface` used by all commands. Builds `OP_MSG` / `OP_COMPRESSED`. |
| `Authenticate.h/.cpp` | High-level authentication orchestration. |
| `AuthenticateScramSha.h/.cpp` | SCRAM-SHA-* implementation (currently the primary mechanism in this codebase). |
| `AuthInfo.h` | Authentication payload types (username/password, certificates, etc.). |
| `AuthClient.h/.cpp` | Client metadata sent to server during handshake (`Auth::Client`). |
| `MongoUtil.h` | Macro machinery for field-access generation and for creating filter/update types (`ThorsMongo_CreateFieldAccess`, `ThorsMongo_FilterFromAccess`, `ThorsMongo_UpdateFromAccess`). Also common enums (op codes, compression, flags). |
| `MongoQuery.h` | Query operator types (e.g. `Eq`, `Gt`, `And`, `Exists`, …) + `Query<T>` wrapper used by remove and some commands. |
| `MongoUpdate.h` | Update operator types (e.g. `Set`, `Inc`, `Unset`, `Push`, …). |
| `ThorsMongoCommon.h/.cpp` | Shared types: `CmdReplyBase`, sort/projection helpers, `ActionConfig<>` base for command configs, cluster time metadata, write error structures. |
| `MongoCursor.h` | Cursor model + generic `Range<R>` and input iterator + `CursorData<T>` lifetime and paging logic. |
| `ThorsMongoInsert.h/.tpp` | `insert` command builder + `InsertResult`. |
| `ThorsMongoFind.h/.tpp` | `find` command builder + `FindRange<T>` and `FindResult<T>` glue. |
| `ThorsMongoGetMore.h/.tpp` | `getMore` paging command. |
| `ThorsMongoKillCursor.h/.tpp` | `killCursors` cleanup command. |
| `ThorsMongoRemove.h/.tpp` | `delete`/remove command builder. |
| `ThorsMongoFindAndModify.h/.tpp` | `findAndModify` family (`findAndReplaceOne`, `findAndRemoveOne`, `findAndUpdateOne`). |
| `ThorsMongoCount.h/.tpp` | `countDocuments` command builder. |
| `ThorsMongoDistinct.h/.tpp` | `distinct` command builder. |
| `ThorsMongoListDatabase.h/.tpp` | `listDatabases` command builder + range wrapper. |
| `ThorsMongoListCollection.h/.tpp` | `listCollections` command builder + range wrapper. |
| `ThorsMongoAdmin.h/.tpp` | Admin commands like `renameCollection`, `drop`, `createCollection`. |
| `ReadConcern.h` / `WriteConcern.h` | Read/write concern payloads. |
| `test/*` | Unit + integration tests and generated mocking hooks. |

---

## Architecture Overview

At a high level, ThorsMongo is a **thin command layer** over:

- **ThorsSerializer** for BSON serialization/deserialization
- **ThorsSocket iostreams** for transport
- A **message handler** that implements MongoDB wire protocol framing

```
┌───────────────────────────────────────────────────────────┐
│                    Application code                        │
│  ThorsMongo / DB / Collection                              │
│  Filters (QueryOp + field access macros)                   │
│  Updates (QueryOp + field access macros)                   │
├───────────────────────────────────────────────────────────┤
│               Command builder objects                       │
│  Insert / Find / Remove / FindAndModify / Distinct / ...    │
├───────────────────────────────────────────────────────────┤
│     MessageHandler (OP_MSG / OP_COMPRESSED / checksum)      │
├───────────────────────────────────────────────────────────┤
│   ConnectionMongo (iostream) + ConnectionBufferMongo        │
│   built on ThorsSocket::BaseSocketStream                    │
├───────────────────────────────────────────────────────────┤
│                       TCP socket                            │
└───────────────────────────────────────────────────────────┘
```

---

## Public Facade Types (Thin Handles)

### `ThorsMongo`

**File:** `ThorsMongo.h`

Owns:

- `MongoMessageHandler messageHandler` (transport + framing)
- default `readConcern` / `writeConcern`
- per-db and per-collection concern overrides (stored in maps)

Exposes:

- `operator[](std::string dbName) -> DB`
- `listDatabases(...) -> DBRange`
- internal cursor helpers: `getMore(...)`, `killCursor(...)` used by cursor-backed ranges

### `DB`

**File:** `ThorsMongo.h`

Lightweight handle consisting of:

- `ThorsMongo& mongoServer`
- `std::string name`

Exposes:

- `operator[](std::string collectionName) -> Collection`
- `listCollections(...) -> LCRange`
- `createCollection(...) -> AdminResult`
- get/set concerns and read preference (delegating to the parent connection and per-db cache)

### `Collection`

**File:** `ThorsMongo.h`

Lightweight handle consisting of:

- `ThorsMongo& mongoServer`
- `std::string name` storing a **combined** db/collection name (`db + "::" + collection`)

Exposes the core data operations:

- `insert`, `find`, `remove`
- `findAndReplaceOne`, `findAndRemoveOne`, `findAndUpdateOne`
- `countDocuments`, `distinct`
- `rename`, `drop`

Also exposes helpers:

- `dbName()` and `colName()` views derived from the combined `name`.

---

## Connection and Streams

### `ConnectionMongo`

**File:** `ConnectionMongo.h`

`ConnectionMongo` is a `std::iostream` (via `ThorsSocket::BaseSocketStream<ConnectionBufferMongo>`) that connects to the MongoDB host/port.

Key points:

- All Mongo wire protocol I/O ultimately happens through this iostream.
- The buffer implementation (`ConnectionBufferMongo`) is responsible for efficient reading/writing and for exposing stream-like semantics to the rest of the stack.

### `MongoMessageHandler`

**File:** `ThorsMongo.h`

Concrete `MessageHandler` that owns a `ConnectionMongo` and returns it via `getStream()`.

This is the bridge between:

- “commands” (objects that can write/read BSON payloads)
- the TCP stream

---

## MessageHandler and Message Framing

### `MongoActionWriteInterface` / `MongoActionReadInterface`

**File:** `MessageHandler.h`

These are the internal “capability” interfaces for any object that can be sent to or received from MongoDB:

- write side must report its BSON size and be able to write itself as BSON
- read side must be able to read itself from BSON

For most command objects and reply objects, ThorsMongo uses trivial adapters:

- `MongoActionWriteInterfaceTrivialImpl<T>` uses `bsonGetPrintSize()` + `bsonExporter()`
- `MongoActionReadInterfaceTrivialImpl<T>` uses `bsonImporter()`

This keeps command classes simple: they just need to be ThorsSerializer-trait’d.

### `MessageHandler`

**File:** `MessageHandler.h/.cpp`

Responsibilities:

- Construct and send MongoDB wire protocol messages (`OP_MSG`)
- Optionally wrap them as `OP_COMPRESSED` depending on negotiated compression
- Optionally append CRC-32C checksums when requested via message flags
- Receive replies and route them back into typed “reply” objects using ThorsSerializer

High-level flow:

```
Command object (traits) ──writeBson()──► MessageHandler ──► ConnectionMongo stream
Reply object   (traits) ◄──readBson()── MessageHandler ◄── ConnectionMongo stream
```

---

## Command Objects (Insert/Find/Remove/etc.)

Each MongoDB command is represented by a small “command object” that:

- contains the command payload (database, collection, filter, etc.)
- is serializable to BSON via ThorsSerializer traits
- is sent via `MessageHandler::sendMessage()`
- receives a typed reply via `MessageHandler::recvMessage()`

Common patterns:

- **Config objects** for optional parameters (e.g. `FindConfig`, `InsertConfig`, …) derived from `ActionConfig<>`
- **Result objects** that inherit from `CmdReplyBase` so they share the standard `isOk()` / `getHRErrorMessage()` / `operator bool()` behavior

Important nuance:

- Some operations return **ranges** rather than a flat “result” (see the cursor-backed design below).

---

## Cursor-backed Ranges

### Cursor model

**File:** `MongoCursor.h`

MongoDB cursors are represented as:

- `Cursor` (metadata: id, namespace, partial results flag)
- `CursorFirst<T>` (first batch of data)
- `CursorNext<T>` (next batches from `getMore`)

Only the `CursorFirst<T>` instance is kept in the long-lived cursor state; when `getMore` returns, `CursorNext<T>` is swapped/moved into `CursorFirst<T>`.

### `Range<R>` and `CursorIterator`

**File:** `MongoCursor.h`

`Range<R>` is the shared implementation used by multiple “range results”:

- `FindRange<T>` (find results)
- list collections/databases ranges

It holds a `std::shared_ptr<R>`; copying the range shares the same cursor state.

Iteration uses a single-pass input iterator:

- `operator*` returns `cursorData.get().get(index)`
- `operator++` advances index and calls `checkAvailable(index)`

### `CursorData<T>` paging and cleanup

**File:** `MongoCursor.h` (class), `ThorsMongo.h` (destructor + paging glue)

`CursorData<T>` owns:

- the `CursorFirst<T>` batch storage
- `getMore` and `killCursor` configs
- back-reference to the owning `ThorsMongo` (so it can call `getMore()` and `killCursor()`)

Two key behaviors:

- **Automatic paging**: when the iterator reaches the end of the current batch, `checkAvailable()` triggers a `getMore()` and swaps in the next batch.
- **Automatic cleanup**: `CursorData<T>::~CursorData()` calls `killCursor()` if the cursor id is non-zero.

This is why `ThorsMongo::getMore()` / `killCursor()` are private but `CursorData<T>` is a friend.

---

## Filters (MongoQuery) and Updates (MongoUpdate)

### Query operator types

**File:** `MongoQuery.h`

Mongo query operators are modeled as small types in `ThorsAnvil::DB::Mongo::QueryOp`:

- comparison: `Eq`, `Ne`, `Gt`, `Gte`, `Lt`, `Lte`, `In`, `Nin`
- logical composition: `And`, `Or`, `Nor`, `Not`
- element: `Exists`, `Type`
- array: `All`, `ElemMatch`, `Size`
- bitwise: `AllClear`, `AllSet`, `AnyClear`, `AnySet`

Each operator type:

- has a nested `using Query = bool;` marker used by concepts
- is ThorsSerializer-trait’d at the bottom of the file so it serializes to the corresponding `$op` key.

### Update operator types

**File:** `MongoUpdate.h`

Updates follow the same idea but emit update documents (`$set`, `$inc`, …). Examples include:

- `Set`, `Unset`, `Inc`, `Min`, `Max`, `Mul`
- array updates: `Push`, `Pull`, `AddToSet`, `PopFront`, `PopBack`, plus helpers like `Each`, `Position`, `Slice`, `Sort`

### Field access + macro generation

**File:** `MongoUtil.h`

The key ergonomics trick is: you declare a “field access” once, and then derive many query/update types from it.

- `ThorsMongo_CreateFieldAccess(Type, field, ...)`
  - generates a `ThorsAnvil::FieldAccess::...::Access<T>` wrapper
  - generates ThorsSerializer overrides so nested field paths become `"a.b.c"` in BSON keys
  - must be used at **global scope** (it creates namespaces and trait specializations)

- `ThorsMongo_FilterFromAccess(Op, Type, field, ...)`
  - wires the field access to a query operator (e.g. `Eq`, `Gt`, …)

- `ThorsMongo_UpdateFromAccess(Op, Type, field, ...)`
  - wires the field access to an update operator (e.g. `Set`, `Inc`, …)

Internally these macros:

- pull the member type from ThorsSerializer’s `Traits<...>` metadata
- normalize `std::optional<T>` fields so you match/update the underlying `T`
- emit the correct dotted field name via `ThorsAnvil_Template_MakeOverride(...)`

---

## Config Objects and Error Model

### `CmdReplyBase`

**File:** `MongoUtil.h` / `ThorsMongoCommon.h`

Most results derive from `CmdReplyBase` which provides:

- `double ok` (Mongo replies use `ok: 1.0` on success)
- `errmsg`, `codeName`, `code`
- `isOk()` and `operator bool()`
- `getHRErrorMessage()` and `operator<<`

This is what enables the idiom:

```cpp
auto result = collection.insert(data);
if (!result) {
    std::cerr << "Error: " << result << "\n";
}
```

### `ActionConfig<T>`

**File:** `ThorsMongoCommon.h`

All command-specific config types derive from `ActionConfig<>`, which centralizes low-level behavior:

- optional `PrinterConfig` / `ParserConfig` overrides (otherwise defaults are used)
- `OP_MsgFlag` bits (checksum, exhaust, etc.)
- `serverErrorAreExceptions` toggle

Command config classes then add their own optional parameters and expose fluent setters.

---

## Concerns and Preferences

ThorsMongo supports setting defaults and overrides at multiple levels:

- **connection-level defaults**: stored in `ThorsMongo`
- **database-level overrides**: cached per db name inside `ThorsMongo`
- **collection-level overrides**: cached per combined `db::collection` name inside `ThorsMongo`

Types involved:

- `ReadConcern` (`ReadConcern.h`)
- `WriteConcern` (`WriteConcern.h`)
- `ReadPreference` (`ThorsMongoCommon.h`)

The `DB` and `Collection` handles implement `get*/set*` by delegating into those maps via the shared owning `ThorsMongo`.

---

## Endianness and BSON

MongoDB wire protocol and BSON are little-endian. The code enforces that at compile time:

- `static_assert(std::endian::little == std::endian::native, ...)` in `ThorsMongo.h`

If big-endian support is needed in the future, all “write integers to wire” helpers in the message framing path need to be audited and endian-corrected.

---

## Header-Only Mode

ThorsMongo can be built in a “header-only” configuration when ThorsSerializer is in header-only mode.

Pattern:

- headers include `*.source` implementations behind:

```cpp
#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
// ...
#include "ThorsMongo.source"
#endif
```

This mirrors the approach used across ThorsAnvil libraries: it avoids separate compilation while keeping a single code path.

---

## Testing and Mocking

Tests live under `third/ThorsMongo/src/ThorsMongo/test/` and include:

- unit tests for query/update macro machinery
- integration tests for connection behavior

Mocking infrastructure is generated into `src/ThorsMongo/coverage/MockHeaders.h` and included by `test/MockHeaderInclude.h`. In non-test builds, mock macros resolve to the real underlying functions; in test builds, they can be redirected to injected lambdas.

