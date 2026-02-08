# NisseHTTP Internal Documentation

Detailed implementation of HTTP parsing, response generation, URL routing, and stream handling in the NisseHTTP library.

**Source:** `third/Nisse/src/NisseHTTP/`

---

## Table of Contents

- [Architecture](#architecture)
- [PyntHTTP Processing Pipeline](#pynthttp-processing-pipeline)
- [Request Parsing](#request-parsing)
- [Response Generation](#response-generation)
- [URL Implementation](#url-implementation)
- [Header Containers](#header-containers)
- [Stream Buffers](#stream-buffers)
- [PathMatcher Implementation](#pathmatcher-implementation)
- [Client-Side HTTP](#client-side-http)
- [Utility Types](#utility-types)
- [File Index](#file-index)

---

## Architecture

NisseHTTP builds on NisseServer to provide HTTP/1.x support:

```
┌──────────────────────────────────────────────────┐
│                  User Application                 │
├──────────────────────────────────────────────────┤
│                   NisseHTTP                       │
│   PyntHTTP · HTTPHandler · Request · Response     │
│   URL · Headers · PathMatcher · Streams           │
├──────────────────────────────────────────────────┤
│                  NisseServer                      │
└──────────────────────────────────────────────────┘
```

---

## PyntHTTP Processing Pipeline

`PyntHTTP` is an HTTP-specific `Pynt` implementation. Its `handleRequest()` method:

1. Constructs a `Request` by parsing the stream.
2. If the request is invalid, sends a 400 response and returns `PyntResult::Done`.
3. Constructs a `Response` (default 200) and calls the pure virtual `processRequest()`.
4. Returns `PyntResult::More` (HTTP keep-alive).

---

## Request Parsing

**Files:** `Request.h`, `Request.cpp`

Parsing happens during `Request` construction:

1. **Read first line**: Method (GET/POST/etc.), path, HTTP version.
2. **Read headers**: Parse into `HeaderRequest` (multi-value map).
3. **Build URL**: Combine `Host` header with request path.
4. **Create body stream**: Based on `Content-Length` or `Transfer-Encoding: chunked`, create the appropriate `StreamInput`.

If any parsing step fails, `failResponse` is populated and `isValidRequest()` returns `false`.

### Request Variables

`RequestVariables` is populated by `HTTPHandler` with:
- HTTP headers (lowercased key)
- URL query parameters
- Path-captured variables
- Form body variables (if `content-type: application/x-www-form-urlencoded`)

All are accessed via `req.variables()["key"]` which returns an empty string on miss.

---

## Response Generation

**Files:** `Response.h`, `Response.cpp`

### Lazy Flushing

The status line and headers are **not** sent until needed:
- `addHeaders()` sends the status line and headers.
- `body()` sends the status line and encoding header.
- The destructor sends a minimal response if nothing was sent.

### Body Encoding

`body(BodyEncoding)` accepts a variant:
- `std::size_t` or `std::streamsize` -- fixed content-length
- `Encoding::Chunked` -- chunked transfer encoding

The returned `std::ostream&` uses a `StreamBufOutput` that handles the encoding transparently.

### Destructor

If neither `addHeaders()` nor `body()` was called, the destructor sends `content-length: 0`. The destructor also logs the response time.

---

## URL Implementation

**Files:** `URL.h`, `URL.cpp`

Parses a URL into components:

```
http://localhost:53/status?name=ryan#234
│       │          │      │          │
protocol origin    path   query      hash
        │
        host (hostname:port)
```

Supports copy, move, swap, and equality comparison. The `param(name)` method parses the query string to find parameter values.

---

## Header Containers

### HeaderRequest

**Files:** `HeaderRequest.h`, `HeaderRequest.cpp`

Stores incoming headers as `std::map<std::string, std::vector<std::string>>`. Supports multiple values per header name (required for headers like `Set-Cookie`).

### HeaderResponse

**Files:** `HeaderResponse.h`, `HeaderResponse.cpp`

Stores outgoing headers as `std::map<std::string, std::string>`. Single value per header.

### HeaderPassThrough

**Files:** `HeaderPassThrough.h`, `HeaderPassThrough.cpp`

Used in reverse-proxy scenarios. Reads headers from an upstream response and writes them directly to the downstream response without full parsing. Detects `Content-Length` and `Transfer-Encoding` to determine body encoding.

---

## Stream Buffers

### StreamBufInput / StreamInput

**Files:** `StreamInput.h`, `StreamInput.cpp`

Custom `std::streambuf` for reading request bodies. Supports:
- **Fixed Content-Length**: reads exactly N bytes then signals EOF.
- **Chunked Transfer-Encoding**: reads chunk headers, decodes chunks, reads trailers on completion.
- `preloadStreamIntoBuffer()`: reads the entire body into memory and returns a `std::string_view`.

### StreamBufOutput / StreamOutput

**Files:** `StreamOutput.h`, `StreamOutput.cpp`

Custom `std::streambuf` for writing response bodies. Supports:
- **Fixed content-length**: writes exactly N bytes.
- **Chunked**: wraps output in HTTP chunk framing (chunk size in hex, CRLF delimiters).
- Handles proper flushing and chunk termination (`0\r\n\r\n`) on destruction.

---

## PathMatcher Implementation

**Files:** `PathMatcher.h`, `PathMatcher.cpp`

### Pattern Decomposition

Each registered path is decomposed into:
- `matchSections` -- literal string segments between captures
- `names` -- names of capture groups
- `method` -- method filter (`Method` or `All`)
- `action` -- function pointer + data

### Matching Algorithm

For a given URL path:
1. Iterate registered patterns.
2. Check method filter.
3. Walk the URL against `matchSections`, extracting characters between literal segments as capture values.
4. URL-decode captured values.
5. Return the first match with its captured variables.

### Action Storage

Actions are stored as a structured type (rather than `std::function` directly) to avoid issues with `dlclose()` on dynamically loaded plugins.

---

## Client-Side HTTP

### ClientStream

**Files:** `ClientStream.h`, `ClientStream.cpp`

Opens a TLS socket connection to a URL. Parses host and port from the URL, establishes an SSL connection.

### ClientRequest

**Files:** `ClientRequest.h`, `ClientRequest.cpp`

Builds and sends an HTTP request. The first line and `Host` header are sent lazily. The destructor calls `flushRequest()`.

### ClientResponse

**Files:** `ClientResponse.h`, `ClientResponse.cpp`

Reads and parses an HTTP response (status line and headers).

---

## Utility Types

**File:** `Util.h`, `Util.cpp`

### StatusCode

Pairs an integer code with its standard message text.

### StandardStatusCodeMap

Singleton lookup table for standard HTTP status codes (200, 201, 301, 400, 404, 500, etc.).

### RequestVariables

A `std::map<std::string, std::string>` wrapper with:
- `exists(key)` -- check if key is present
- `operator[](key)` -- returns empty string on miss
- `insert_or_assign(key, value)` -- insert or update
- Iteration support

### Header Variant

```cpp
using Header = std::variant<HeaderResponse const&, HeaderPassThrough const&>;
```

Allows `response.addHeaders()` to accept either header type.

### BodyEncoding Variant

```cpp
using BodyEncoding = std::variant<std::size_t, std::streamsize, Encoding>;
```

---

## HTTPHandler Request Processing

When `HTTPHandler::processRequest()` is called:

1. Normalize the URL path.
2. Delegate to `PathMatcher::findMatch()`.
3. If a match is found:
   a. Populate `request.variables()` with: HTTP headers, URL query parameters, path-captured variables, and form body variables.
   b. Call the validation function; send 400 on failure.
   c. Call the user action.
4. If no match: send 404.

---

## File Index

| File | Description |
|------|-------------|
| `PyntHTTP.h/.cpp` | HTTP implementation of Pynt |
| `PyntHTTPControl.h/.cpp` | HTTP control endpoint |
| `HTTPHandler.h/.cpp` | URL-pattern-based route dispatcher |
| `Request.h/.cpp` | HTTP request parser |
| `Response.h/.cpp` | HTTP response builder |
| `URL.h/.cpp` | URL parser |
| `HeaderRequest.h/.cpp` | Incoming header container |
| `HeaderResponse.h/.cpp` | Outgoing header container |
| `HeaderPassThrough.h/.cpp` | Proxy-style header forwarding |
| `HeaderStreamOperator.tpp` | Variant stream operator for Header type |
| `StreamInput.h/.cpp` | Input streambuf (content-length / chunked) |
| `StreamOutput.h/.cpp` | Output streambuf (content-length / chunked) |
| `PathMatcher.h/.cpp` | URL pattern matching with captures |
| `Util.h/.cpp` | Enums, StatusCode, RequestVariables |
| `ClientStream.h/.cpp` | Outbound TLS connection |
| `ClientRequest.h/.cpp` | Outbound HTTP request builder |
| `ClientResponse.h/.cpp` | Inbound HTTP response parser |
